# RAIL Proprietary - OTA DFU Target #

## Overview ##

This example receives a GBL image over a simple proprietary RAIL link, stores
it into a bootloader storage slot, validates the received image, and then lets
the user either install it with PB0 or erase the slot with PB1.

The current target implementation is intentionally minimal:

- it accepts one fixed packet format,
- it expects strictly ordered segment IDs,
- it returns a 1-byte application result in the auto-ACK payload,
- it validates the received image before installation,
- it does not install automatically after validation.

> [!WARNING]
> This example is not intended to be production-ready OTA DFU code. Its main
> purpose is to demonstrate how RAIL Auto-ACK can be used for large data
> transfer in a realistic scenario. The bootloader image is used here primarily
> as a convenient large payload, not as the main feature being demonstrated,
> and the bootloader APIs may therefore be used in ways that are acceptable for
> a focused example but are not necessarily optimal for a production design.

## SDK Version ##

SiSDK 2025.6.0 and above

## Hardware Required ##

- Two EFR32 Series 2 development kits, or equivalent supported boards.
- One board running the target example and one board running the matching host example.
- A bootloader storage configuration with at least one storage slot on both the host and target.
- Push buttons (PB0 and PB1) and a VCOM log connection on the target board, and a push button (PB0) plus a VCOM log connection on the host board.
- Optionally, a logic analyzer connected to the debug pins.

### Tested Boards for This Example ###

| Board ID | Description                                                                                                                                                                           |
|----------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| BRD4182A | [EFR32xG22 2.4 GHz 6 dBm Radio Board](https://www.silabs.com/development-tools/wireless/slwrb4182a-efr32xg22-wireless-gecko-radio-board?tab=overview)                                 |
| BRD4204D | [EFR32ZG23 868-915 MHz 14 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg23-rb4204d-efr32xg23-868-915-mhz-14-dbm-radio-board?tab=overview)                      |
| BRD4186C | [EFR32xG24 2.4 GHz 10 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg24-rb4186c-efr32xg24-wireless-gecko-radio-board?tab=overview)                              |

## Connections Required ##

- Connect both the host and target kits to the PC over USB for programming and
  VCOM logging.
- No direct wired link is required between them. Segments are transferred over
  the configured radio PHY.

## Setup ##

1. Open [SimplicityStudio/rail_ota_dfu_target.slcp](./SimplicityStudio/rail_ota_dfu_target.slcp)
   and [../rail_ota_dfu_host/SimplicityStudio/rail_ota_dfu_host.slcp](../rail_ota_dfu_host/SimplicityStudio/rail_ota_dfu_host.slcp)
   in Simplicity Studio, or generate equivalent projects with the same
   components and radio configuration.
2. Adjust parameters detailed in the [Configuration](#configuration) section
   below.
3. Program bootloaders with at least one storage slot on both devices.
4. Build and flash the target application to the target board and the host
   application to the second board.
5. Make sure both projects use compatible radio settings and matching values
   for segment length and default channel.
6. Preload the target GBL into the host storage slot as described in the host
   README.
7. Boot the target and check whether it already contains a valid image. If it
   does, press PB1 to erase the slot before starting a new transfer.

## How It Works ##

During initialization, the target configures its Tx FIFO and auto-ACK handling.
After that, the state machine validates the configured target storage slot.

Two startup paths are possible:

- If a valid image is already present, the target idles the radio, logs the
  validated image size, and waits in the idle state for user action.
- If no valid image is present, or if the slot is erased because of PB1 or a
  fatal error, the target erases the configured slot, starts RX on the default
  channel, and waits for segments from the host.

The on-air packet format is fixed length:

- a 2-byte segment header,
- followed by `SL_OTA_DFU_TARGET_SEGMENT_LENGTH` bytes of image payload.

The segment header encodes:

- bit 15: last-segment marker,
- bits 14:0: segment ID.

When a packet arrives, the target copies it from the RAIL event handler into a
local buffer and evaluates the segment ID:

1. If the segment ID matches the next expected ID, the target prepares a
   success ACK immediately, then writes the payload into the bootloader slot.
2. If the packet repeats the previous segment ID, the target sends a success
   ACK again so the host can continue after a missed ACK.
3. If the segment ID is unexpected, the target sends a failure ACK and keeps
   waiting for the expected segment.

The current ACK payload values are:

- `0x01`: segment accepted,
- `0x00`: segment rejected.

When the last-segment marker is received and the stored image validates
successfully, the target returns to idle and prompts the user:

- PB0 installs the validated image,
- PB1 erases the slot and restarts receive mode.

## How to Use ##

1. Boot the target and watch the VCOM log.
2. If the target reports a validated image already in the slot, decide whether
   to install it with PB0 or erase it with PB1.
3. If the slot is empty or has just been erased, confirm that the target is
   waiting for the host to initiate DFU.
4. Start the transfer from the host by pressing PB0 on the host board.
5. Wait until the target log reports that the received image was validated.
6. Press PB0 on the target board to install the validated image, or PB1 to
   erase the slot and return to receive mode.

## Configuration ##

### Peripherals Used ###

The current target implementation uses:

- the default VCOM-backed log stream,
- PB0 to install a validated image,
- PB1 to erase the slot and restart reception,
- the configured bootloader storage slot as the destination image store,
- the `PRS_RACL_RX` and `PRS_RACL_TX` signals are available at the
  `EXP_HEADER_15` and `EXP_HEADER_16` debug pins on Wireless Motherboards,
  respectively.

### Project Specific Configurations ###

Main configuration file:

- `<project_root>/config/sl_rail_ota_dfu_target_config.h`

#### `SL_OTA_DFU_TARGET_SEGMENT_LENGTH` ####

Defines the number of image bytes received per segment packet.

- It must match the host configuration.
- The effective packet length on air is `SL_OTA_DFU_TARGET_SEGMENT_LENGTH + 2`
  bytes.

#### `SL_OTA_DFU_TARGET_TX_FIFO_LENGTH` ####

Defines the Tx FIFO size used by the ACK path.

#### `SL_OTA_DFU_TARGET_DEFAULT_CHANNEL` ####

Defines the radio channel used to receive segments and return auto-ACK
payloads.

- It must match the host configuration.

#### `SL_OTA_DFU_TARGET_DEFAULT_SLOT_ID` ####

Selects the bootloader storage slot that receives the transferred GBL image.

## Notes ##

- Duplicate packets for the previously accepted segment are ACKed again so the
  host can recover from a missed ACK.
- Invalid-image startup, PB1 erase requests, and fatal bootloader errors all
  follow the same reset path: idle the radio, erase the slot, and restart RX.
- The application prepares a success ACK before `bootloader_writeStorage()`
  completes. If the write fails afterward, the host may already have advanced
  to the next segment, while the target follows the reset path.
- The target checks only its own configured slot size. There is no explicit
  protocol negotiation of host and target storage capacity.
- The protocol is intentionally minimal. There is no session ID, device ID,
  authentication, encryption, or resume-from-offset feature.
- Installation is manual after validation. The application does not
  automatically reboot into the bootloader when the last segment arrives.
- The radio PHY is not otherwise constrained, because the application switches
  the packet length dynamically between ACK and segment traffic.
- Slot size mismatches between the host and target are not handled; the user
  must ensure they are compatible.
- Host-side handling of a final-segment ACK reception failure is incomplete.
  An RX ACK failure, as distinct from a timeout, is not retried correctly.
