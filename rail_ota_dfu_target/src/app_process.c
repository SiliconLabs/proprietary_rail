/***************************************************************************//**
 * @file
 * @brief app_process.c
 *****************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
 *****************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Experimental Quality
 * This code has not been formally tested and is provided as-is. It is not
 * suitable for production environments. In addition, this code will not be
 * maintained and there may be no bug maintenance planned for these resources.
 * Silicon Labs may update projects from time to time.
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_component_catalog.h"
#include "sl_rail.h"
#include "sl_rail_util_init.h"
#include "sl_simple_button.h"
#include "sl_simple_button_instances.h"

#include "btl_errorcode.h"
#include "btl_interface.h"

#include "app_log.h"
#include "app_process.h"
#include "ota_dfu_target.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
// Segment payload length includes the 2-byte segment ID.
#define SEGMENT_PAYLOAD_LENGTH  (SL_OTA_DFU_TARGET_SEGMENT_LENGTH + 2)
#define ACK_PAYLOAD_LENGTH      1U

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/****************************************************************************//*
 * Logs the reason, resets storage and state, and restarts RX so a new image
 * can be received.
 *
 * Call this from the state machine and follow it with a break statement.
 *
 * @param[in] message Error message to log.
 ******************************************************************************/
static void reset_state_machine(const char *message);

/****************************************************************************//*
 * Prepares the ACK payload content and length.
 *
 * @param[in] success Indicates whether the ACK is for a successful reception or
 * not.
 ******************************************************************************/
static void prepare_ack(bool success);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// OTA DFU state. On boot, validate the stored image before going idle or
// restarting reception.
static volatile sl_rail_ota_dfu_target_state_t ota_dfu_state =
  sl_rail_ota_dfu_target_state_validate_image;

// Receive buffer with 2 extra bytes for the segment ID.
static uint8_t rx_buffer[SEGMENT_PAYLOAD_LENGTH];

// ACK payload buffer.
static uint8_t tx_ack_payload[ACK_PAYLOAD_LENGTH];

// Set when a packet has been received and is ready to process.
static volatile bool packet_received_flag = false;

// Set when an Rx error occurs.
static volatile bool receive_error_flag = false;

// Set when a calibration error occurs.
static volatile bool calibration_error_flag = false;

// Next segment ID expected from the host.
static uint16_t next_segment_id = 0;

// Image size after validation, or an invalid marker if no valid image exists.
static uint32_t image_size;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(void)
{
  // RAIL handle used by the application.
  sl_rail_handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // RAIL API status.
  sl_rail_status_t rail_status;

  // Status for bootloader operations.
  int32_t bootloader_status;

  // Current segment ID from the received packet.
  static uint16_t segment_id = 0;

  // Set when the current packet carries the last-segment flag.
  static bool last_segment = false;

  switch (ota_dfu_state) {
    case sl_rail_ota_dfu_target_state_validate_image:
      // Retrieve the stored image size.
      image_size = sl_rail_ota_dfu_get_image_size();
      if (image_size == SL_RAIL_OTA_DFU_INVALID_IMAGE_SIZE) {
        // No valid image is available; restart reception.
        reset_state_machine("No image stored, or stored image is invalid");
        break;
      } else {
        // After a transfer completes, idle the radio before going back to idle.
        rail_status = sl_rail_idle(rail_handle, SL_RAIL_IDLE_ABORT, true);
        if (rail_status != SL_RAIL_STATUS_NO_ERROR) {
          app_log_error("Failed to idle the radio.\n");
        }
        // Inform the user that a valid image is ready.
        app_log(
          "Validated image size: %lu bytes. Press PB0 to install it, or PB1 to erase the slot.\n",
          image_size);
        ota_dfu_state = sl_rail_ota_dfu_target_state_idle;
      }
      break;

    case sl_rail_ota_dfu_target_state_idle:
      // Wait for user input while the radio stays idle.
      break;

    case sl_rail_ota_dfu_target_state_wait_segment:

      // Recover from RX errors.
      if (receive_error_flag) {
        receive_error_flag = false;
        // Restore the expected segment packet length.
        sl_rail_set_fixed_length(rail_handle,
                                 SL_OTA_DFU_TARGET_SEGMENT_PACKET_LENGTH);
      }

      // Process a newly received packet.
      if (packet_received_flag) {
        // Clear the flag.
        packet_received_flag = false;
        // Decode the segment ID from the low 15 bits of the header.
        segment_id = (((uint16_t)rx_buffer[0] << 8) | rx_buffer[1]) & 0x7FFF;
        // Decode the last-segment flag from the header MSB.
        last_segment = (rx_buffer[0] & 0x80) != 0;
        // Accept the expected segment.
        if (segment_id == next_segment_id) {
          prepare_ack(true); // Time-critical.
          next_segment_id++;
          ota_dfu_state = sl_rail_ota_dfu_target_state_process_segment;
          break;
        } else {
          // Re-ACK the previous segment in case the host missed the ACK.
          if (segment_id == next_segment_id - 1) {
            prepare_ack(true); // Time-critical.
          } else {
            // Reject an unexpected segment so the host retries or aborts.
            prepare_ack(false); // Time-critical.
            app_log_warning(
              "Received unexpected segment ID: %u. Expected: %u. Consider erasing the slot.\n",
              segment_id,
              next_segment_id);
          }
        }
      }
      break;

    case sl_rail_ota_dfu_target_state_process_segment:

      // Write the received segment into storage.
      bootloader_status =
        sl_rail_ota_dfu_target_write_segment(segment_id, &rx_buffer[2],
                                             SL_OTA_DFU_TARGET_SEGMENT_LENGTH);
      if (bootloader_status != BOOTLOADER_OK) {
        // Bootloader write failures are fatal for the current transfer.
        reset_state_machine("Failed to write to bootloader storage");
        break;
      }

      // Select the next state based on the last-segment flag.
      if (last_segment) {
        ota_dfu_state = sl_rail_ota_dfu_target_state_validate_image;
      } else {
        ota_dfu_state = sl_rail_ota_dfu_target_state_wait_segment;
      }
      break;

    case sl_rail_ota_dfu_target_state_install_image:

      // Mark the new image for boot and reboot to install it.
      bootloader_status =
        bootloader_setImageToBootload(SL_OTA_DFU_TARGET_DEFAULT_SLOT_ID);
      if (bootloader_status != BOOTLOADER_OK) {
        // Bootloader failures are fatal here as well.
        reset_state_machine("Failed to mark image for boot");
        break;
      }
      bootloader_rebootAndInstall();

      // The device should reboot before reaching this point.
      reset_state_machine("Rebooting to install image");
      break;

    case sl_rail_ota_dfu_target_state_erase_slot:
      // Erase the slot and restart reception.
      reset_state_machine("User requested slot erase");
      break;

    default:
      break;
  }

  if (calibration_error_flag) {
    calibration_error_flag = false;
    app_log_error("sl_rail_calibrate was unable to perform calibration!");
  }
}

/******************************************************************************
 * RAIL callback. Called when a RAIL event occurs.
 *****************************************************************************/
void sl_rail_util_on_event(sl_rail_handle_t rail_handle,
                           sl_rail_events_t events)
{
  if (events & SL_RAIL_EVENTS_RX_COMPLETION) {
    if (events & SL_RAIL_EVENT_RX_PACKET_RECEIVED) {
      // Read the packet in the interrupt handler so the ACK can be prepared
      //   quickly.
      sl_rail_rx_packet_info_t packet_info;
      sl_rail_rx_packet_handle_t packet_handle;
      sl_rail_status_t status;

      packet_handle = sl_rail_get_rx_packet_info(rail_handle,
                                                 SL_RAIL_RX_PACKET_HANDLE_NEWEST,
                                                 &packet_info);
      if (packet_handle != SL_RAIL_RX_PACKET_HANDLE_INVALID) {
        status = sl_rail_copy_rx_packet(rail_handle, rx_buffer, &packet_info);
        if (status == SL_RAIL_STATUS_NO_ERROR) {
          packet_received_flag = true;
        } else {
          // Treat a copy failure as an RX error.
          receive_error_flag = true;
        }
      } else {
        // This should not happen, but recover as an RX error.
        receive_error_flag = true;
      }
    } else {
      receive_error_flag = true;
    }
  }

  if (events & SL_RAIL_EVENTS_TXACK_COMPLETION) {
    // Time-critical: restore the segment reception frame length immediately.
    sl_rail_set_fixed_length(rail_handle,
                             SL_OTA_DFU_TARGET_SEGMENT_PACKET_LENGTH);
  }

  if (events & SL_RAIL_EVENT_CAL_NEEDED) {
    if (SL_RAIL_STATUS_NO_ERROR != sl_rail_calibrate(rail_handle,
                                                     NULL,
                                                     SL_RAIL_CAL_ALL_PENDING)) {
      calibration_error_flag = true;
    }
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)

  app_task_notify();
#endif
}

/******************************************************************************
 * Button callback. Called when a button event occurs.
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  // PB0 installs the validated image.
  if ((handle == SL_SIMPLE_BUTTON_INSTANCE(0))
      && (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED)) {
    if ((image_size != SL_RAIL_OTA_DFU_NOT_DETERMINED_IMAGE_SIZE)
        && (image_size != SL_RAIL_OTA_DFU_INVALID_IMAGE_SIZE)) {
      ota_dfu_state = sl_rail_ota_dfu_target_state_install_image;
    } else {
      // Logging here can interfere with an active transfer.
      app_log_warning("No valid image is available for installation.\n");
    }
  }

  // PB1 erases the slot and restarts reception.
  if ((handle == SL_SIMPLE_BUTTON_INSTANCE(1))
      && (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED)) {
    ota_dfu_state = sl_rail_ota_dfu_target_state_erase_slot;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void reset_state_machine(const char *message)
{
  sl_rail_status_t status;
  sl_rail_handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  int32_t bootloader_status;

  // Log the reset reason.
  app_log_warning("Resetting state machine:\n  %s\n", message);

  // Always idle the radio first.
  status = sl_rail_idle(rail_handle, RAIL_IDLE_ABORT, true);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    app_log_error("Failed to put the radio into idle: %ld\n", status);
  }

  // Reset state variables.
  next_segment_id = 0;
  image_size = SL_RAIL_OTA_DFU_NOT_DETERMINED_IMAGE_SIZE;
  receive_error_flag = false;
  packet_received_flag = false;

  // Erase the storage slot.
  app_log("Erasing the storage slot.\n");
  bootloader_status =
    bootloader_eraseStorageSlot(SL_OTA_DFU_TARGET_DEFAULT_SLOT_ID);
  if (bootloader_status != BOOTLOADER_OK) {
    app_log_error("Failed to erase the storage slot: %ld\n", bootloader_status);
  } else {
    app_log("Storage slot erased.\n");
  }

  // Restart RX with the expected frame length.
  sl_rail_set_fixed_length(rail_handle,
                           SL_OTA_DFU_TARGET_SEGMENT_PACKET_LENGTH);
  status = sl_rail_start_rx(rail_handle, SL_OTA_DFU_TARGET_DEFAULT_CHANNEL,
                            NULL);
  if (status != SL_RAIL_STATUS_NO_ERROR) {
    app_log_error("Failed to start RX: %ld\n", status);
  }

  // Return to the wait-for-segment state.
  ota_dfu_state = sl_rail_ota_dfu_target_state_wait_segment;

  app_log("Waiting for the host to initiate DFU.\n");
}

static void prepare_ack(bool success)
{
  sl_rail_handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
  // The ACK tells the host whether to continue, retry, or abort.
  tx_ack_payload[0] =
    success ? SL_OTA_DFU_TARGET_ACK_OK : SL_OTA_DFU_TARGET_ACK_FAIL;
  // Configure the fixed payload length for the ACK packet.
  sl_rail_set_fixed_length(rail_handle, ACK_PAYLOAD_LENGTH);
  // Write the auto-ACK payload.
  sl_rail_write_auto_ack_fifo(rail_handle, tx_ack_payload, ACK_PAYLOAD_LENGTH);
}
