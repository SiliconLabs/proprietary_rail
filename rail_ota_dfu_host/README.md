# RAIL Proprietary - OTA DFU Host #

## Overview ##

This example sends a GBL image from a bootloader storage slot on the host node
to the [RAIL Proprietary - OTA DFU Target](../rail_ota_dfu_target) over a
simple proprietary RAIL link.

The current implementation is intentionally narrow in scope:

- the source GBL must already be present in the host bootloader slot,
- the radio packet format is fixed length,
- each packet contains a 2-byte segment header followed by a fixed payload,
- retransmission is driven by a 1-byte application result carried in the
  target auto-ACK payload,
- the transfer starts only when PB0 is pressed on the host.

> [!WARNING]
> This example is not intended to be production-ready OTA DFU code. Its main
> purpose is to demonstrate how RAIL Auto-ACK can be used for large data
> transfer in a realistic scenario. The bootloader image is used here primarily
> as a convenient large payload, not as the core feature being demonstrated,
> and the bootloader APIs may therefore be used in ways that are acceptable for
> a focused example but are not necessarily optimal for a production design.

## SDK Version ##

SiSDK 2025.6.0 and above

## Hardware Required ##

- Two EFR32 Series 2 development kits, or equivalent supported boards.
- One board running the host example and one board running the matching target example.
- A bootloader storage configuration with at least one storage slot on both the host and target.
- A push button (PB0) and a VCOM log connection on the host board, and push buttons (PB0 and PB1) plus a VCOM log connection on the target board.
- Optionally, a logic analyzer connected to the debug pins.

### Tested Boards for This Example ###

| Board ID | Description                                                                                                                                                                           |
|----------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| BRD4182A | [EFR32xG22 2.4 GHz 6 dBm Radio Board](https://www.silabs.com/development-tools/wireless/slwrb4182a-efr32xg22-wireless-gecko-radio-board?tab=overview)                                 |
| BRD4204D | [EFR32ZG23 868-915 MHz 14 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg23-rb4204d-efr32xg23-868-915-mhz-14-dbm-radio-board?tab=overview)                      |
| BRD4186C | [EFR32xG24 2.4 GHz 10 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg24-rb4186c-efr32xg24-wireless-gecko-radio-board?tab=overview)                              |

## Connections Required ##

- Connect both kits to the PC over USB for programming and VCOM logging.
- No wired connection is required between the host and target boards. Image
  transfer happens over the configured radio PHY.

## Setup ##

1. Open [SimplicityStudio/rail_ota_dfu_host.slcp](./SimplicityStudio/rail_ota_dfu_host.slcp)
   and [../rail_ota_dfu_target/SimplicityStudio/rail_ota_dfu_target.slcp](../rail_ota_dfu_target/SimplicityStudio/rail_ota_dfu_target.slcp)
   in Simplicity Studio, or generate equivalent projects with the same
   components and radio configuration.
2. Adjust parameters detailed in the [Configuration](#configuration) section
   below.
3. Program bootloaders with at least one storage slot on both devices.
4. Build and flash the host application to one board and the target
   application to the other.
5. Make sure both projects use compatible radio settings and matching values
   for segment length and default channel.
6. Generate a GBL image from the target application build output.
7. Write that GBL image into the host storage slot selected by
   `SL_OTA_DFU_HOST_DEFAULT_SLOT_ID`.

The exact programming command depends on the bootloader storage implementation.
For an SPI flash storage bootloader, for example:

```text
commander gbl create <target-image>.gbl --app <target-application>
commander extflash erase --range <bootloader-storage-range>
commander extflash write <target-image>.gbl --address <slot-start-address>
```

## How It Works ##

At startup, the host prints the selected storage slot address and length, then
scans that slot for a valid GBL v3 image by walking the tag headers until
`GBL_TAG_ID_END` is found.

If a valid image is present, the host derives its transfer state from the
parsed image size and `SL_OTA_DFU_HOST_SEGMENT_LENGTH`. The on-air packet
format is fixed length:

- a 2-byte segment header,
- followed by `SL_OTA_DFU_HOST_SEGMENT_LENGTH` bytes of image payload.

The segment header uses:

- bit 15 as the last-segment marker,
- bits 14:0 as the segment ID.

When PB0 is pressed while the host is idle, the host:

1. reads the next payload chunk directly from the configured bootloader slot,
2. prepends the 2-byte segment header,
3. transmits the packet with `SL_RAIL_TX_OPTION_WAIT_FOR_ACK`,
4. waits for the target auto-ACK payload,
5. advances on ACK success,
6. resends the same segment on ACK failure or ACK timeout.

The target ACK payload is currently one byte:

- `0x01`: segment accepted,
- `0x00`: segment rejected.

The host retries each segment up to
`SL_OTA_DFU_HOST_SEGMENT_RETRY_COUNT` times before aborting the transfer.
The ACK timeout is configured by `SL_OTA_DFU_HOST_ACK_TIMEOUT_US`.

## How to Use ##

1. Boot the target board and confirm that it is ready to receive a new image.
   If the target reports that a valid image is already present, press PB1 on
   the target to erase the slot and restart receive mode.
2. Boot the host board and check the VCOM log for storage slot information and
   source image detection.
3. Press PB0 on the host board to start the transfer.
4. Monitor the host log for progress, retries, and completion.
5. Monitor the target log for slot erase, reception, and image validation.
6. After the target validates the image, press PB0 on the target board to mark
   the received image for boot and reboot into the bootloader install flow.

## Configuration ##

### Peripherals Used ###

The current host implementation uses:

- the default VCOM-backed log stream,
- PB0 to start a transfer,
- the configured bootloader storage slot as the source image store,
- the `PRS_RACL_RX` and `PRS_RACL_TX` signals are available at the
  `EXP_HEADER_15` and `EXP_HEADER_16` debug pins on Wireless Motherboards,
  respectively.

### Project Specific Configurations ###

Main configuration file:

- `<project_root>/config/sl_rail_ota_dfu_host_config.h`

#### `SL_OTA_DFU_HOST_SEGMENT_LENGTH` ####

Defines the number of image bytes carried in one radio packet.

- It must match `SL_OTA_DFU_TARGET_SEGMENT_LENGTH` on the target.
- The effective packet length on air is `SL_OTA_DFU_HOST_SEGMENT_LENGTH + 2`
  bytes.
- Because the segment ID is 15 bits wide, the maximum representable image span
  is `SL_OTA_DFU_HOST_SEGMENT_LENGTH * 0x7FFF` bytes.

#### `SL_OTA_DFU_HOST_TX_FIFO_LENGTH` ####

Defines the Tx FIFO size.

- It must be large enough for the fixed segment packet length.

#### `SL_OTA_DFU_HOST_DEFAULT_CHANNEL` ####

Defines the radio channel used for the transfer.

- It must match the target configuration.

#### `SL_OTA_DFU_HOST_DEFAULT_SLOT_ID` ####

Selects the bootloader storage slot that already contains the source GBL image.

#### `SL_OTA_DFU_HOST_ACK_TIMEOUT_US` ####

Defines the auto-ACK timeout used while waiting for the target response.

#### `SL_OTA_DFU_HOST_SEGMENT_RETRY_COUNT` ####

Defines how many times the host resends one segment before aborting the
transfer.

## Notes ##

- The current implementation assumes the source image is already present in the
  configured host storage slot before the application starts.
- If no valid image is found at startup, the application logs an error but does
  not fully disable transfer initiation afterward.
- The protocol is intentionally minimal. There is no session ID, device ID,
  authentication, encryption, or resume-from-offset feature.
- The host always transmits fixed-length packets. The true image size is
  reconstructed from GBL parsing rather than from an explicit image-size field
  in the protocol.
- The code derives `last_segment_id` from the parsed image size and segment
  length, but later uses it as a terminal segment ID. Progress reporting and
  the displayed block count therefore do not currently represent a proven exact
  segment count, and final-segment edge cases should be validated before
  production use.
