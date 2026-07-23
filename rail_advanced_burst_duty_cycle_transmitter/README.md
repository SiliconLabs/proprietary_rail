# RAIL Proprietary - Advanced Burst Duty Cycle Transmitter #

## Overview ##

This project showcases a Tx-only application using RAIL to transmit burst duty
cycle packets without gaps. Each burst packet contains timestamp information for
a scheduled data packet, allowing a receiver to wake up at the right time upon
receiving any burst. This approach improves efficiency by enabling precise
synchronization while minimizing power consumption.

To get more information about the topic of low duty cycle, read our [related
article](https://docs.silabs.com/rail/latest/rail-training-low-duty-cycle/).

> [!IMPORTANT]  
> This application lacks error handling and other critical features for
> readability. It is intended solely for educational purposes and is not
> recommended as a foundation for development.

## SDK version ##

SiSDK 2025.6.0 and above

## Hardware Required ##

EFR32 Series 2

## Connections Required ##

Connect two Development Kits to your PC.

### Tested boards for working with this example ###

| Board ID | Description  |
| ---------------------- | ------ |
| BRD4182A | [EFR32xG22 2.4 GHz 6 dBm Radio Board](https://www.silabs.com/development-tools/wireless/slwrb4182a-efr32xg22-wireless-gecko-radio-board?tab=overview) |
| BRD4204D | [EFR32ZG23 868-915 MHz 14 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg23-rb4204d-efr32xg23-868-915-mhz-14-dbm-radio-board?tab=overview)|
| BRD4187C | [EFR32xG24 Wireless 2.4 GHz +20 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg24-rb4187c-efr32xg24-wireless-gecko-radio-board?tab=overview)|
| BRD4270B | [EFR32FG25 902-928 MHz +16 dBm Radio Board](https://www.silabs.com/development-tools/wireless/proprietary/fg25-rb4270b-efr32fg25-radio-board?tab=overview) |
| BRD4194A | [EFR32xG27 Wireless 2.4 GHz +8 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg27-rb4194a-efr32xg27-8-dbm-wireless-radio-board?tab=overview) |
| BRD4401C | [EFR32xG28 868/915 MHz 20 dBm + 2.4 GHz 10 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg28-rb4401c-efr32xg28-2-4-ghz-ble-and-20-dbm-radio-board?tab=overview) |

## Setup ##

1. Generate Project with [Simplicity Studio
   v5](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#example-projects-demos-tab)
   or with [SLC
   CLI](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-tools-slc-cli/).
2. Adjust parameters detailed in the [Configuration](#configuration) section
   below.
3. Build and flash the project on a board.

## How It Works ##

This project demonstrates how to transmit burst duty cycle packets with
timestamping information of a data packet with RAIL. The example uses two packet
types: the burst packet - which is repeated over and over with a minimal change
in the payload that contains the timestamp information when the data packet is
expected, and the data packet itself. To use this application you need two
nodes, one with the transmitter and one with the receiver program.

This program provides information via the terminal, including success status of
a transmission sequence and instruction to operate the application.

Note, that this is not a DMP (Dynamic Multiprotocol) project! The functional
part of the code is separated to make sure that it can be used in DMP projects
if needed.

> `SL_ADVANCED_BURST_DC_TRANSMITTER_DATA_PAYLOAD_LEN` is a variable used to
> define the payload length for transmission in the anchored data frame. With
> this value, the size of data_frame_t must be smaller than
> `SL_ADVANCED_BURST_DC_TRANSMITTER_BUFFER_LENGTH`. The length of the Tx buffer
> should be set according to the radio configuration using this parameter. The
> `SL_ADVANCED_BURST_DC_TRANSMITTER_DEFAULT_CHANNEL` parameter determines the
> index of the channel that is used for packet transmission (see available
> channels in the radio_settings.radioconf file of the project). The CRC
> polynomial used can be configured with
> `SL_ADVANCED_BURST_DC_TRANSMITTER_CRC_POLYNOMIAL` and the CRC seed with
> `SL_ADVANCED_BURST_DC_TRANSMITTER_CRC_SEED`. Although no DMP is used in this
> example, the rail_packet_streamer supports this functionality, so the
> parameters can be configured with
> `SL_ADVANCED_BURST_DC_TRANSMITTER_DMP_PRIORITY` and
> `SL_ADVANCED_BURST_DC_TRANSMITTER_DMP_SLIPTIME_US`. The preamble properties
> can be changed by `SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_PATTERN`,
> `SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_LEN` and
> `SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_PATT_LEN`. The timestamp of the
> data packet can be configured by
> `SL_ADVANCED_BURST_DC_TRANSMITTER_ANCHOR_DELAY_US`, which is the amount on
> time in microseconds between the end of the burst packets and the start of the
> data packet. This delay can be calibrated with
> `SL_ADVANCED_BURST_DC_TRANSMITTER_SCHEDULE_DELAY_US`. The length of the burst
> is determined by `SL_ADVANCED_BURST_DC_TRANSMITTER_BURST_LEN_MS`.

### How to use ###

To initiate a transmission sequence, you need to press the BTN0 button.

### Configuration ###

#### Peripherals Used ####

The application utilizes the default EUSART instance for CLI communication and
console logging. The VCOM enable signal is pre-configured for the tested boards.

You may want to adjust the EUSART (VCOM) configurations according to the
board you are using.

It also uses the GPCRC peripheral to be able to add CRC values to the Tx frame.
The CRC parameters can be configured with the related macros in the config file,
so the radio configurator values are ignored.

#### Project Specific Configurations ####

Additional project-specific configurations can be found in the
`<project_root>/config/sl_rail_advanced_burst_dc_transmitter_config.h` file.
These configurations are not currently accessible through the UI as component
settings, so you will need to modify them manually using a text editor or IDE.

#### `SL_ADVANCED_BURST_DC_TRANSMITTER_DATA_PAYLOAD_LEN` ####

Payload length in bytes of the data message inside data_frame_t. The minimum
length is 16 as the app uses a fixed 16 lengh payload for the data message.

#### `SL_ADVANCED_BURST_DC_TRANSMITTER_BUFFER_LENGTH` ####

Sets Tx Buffer length in bytes.

- [The size must be a power of 2 from 64 to
  4096](https://docs.silabs.com/rail/latest/rail-api/efr32-main#receive-and-transmit-fifo-buffers)

#### `SL_ADVANCED_BURST_DC_TRANSMITTER_DEFAULT_CHANNEL` ####

Set the index of the default channel used to transmit.

- Only configured channel index can be used (see available channels in the
  radio_settings.radioconf file of the project).

#### `SL_ADVANCED_BURST_DC_TRANSMITTER_CRC_POLYNOMIAL` ####

The used CRC polynomial value. To calculate this see GPCRC module.

#### `SL_ADVANCED_BURST_DC_TRANSMITTER_CRC_SEED` ####

The initial value for the CRC calculation.

#### `SL_ADVANCED_BURST_DC_TRANSMITTER_DMP_PRIORITY` ####

DMP priority to use for wakeup Tx. 0 is highest priority.

#### `SL_ADVANCED_BURST_DC_TRANSMITTER_DMP_SLIPTIME_US` ####

Allowed delay to start a wakeup transmission, to help RAIL scheduler to schedule
the protocols. In microseconds.

#### `SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_PATTERN` ####

Set the preamble pattern to use. This should match Preamble Base Pattern field
configured on the Radio Config GUI

#### `SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_PATT_LEN` ####

Length of the preamble pattern in bits.

#### `SL_ADVANCED_BURST_DC_TRANSMITTER_PREAMBLE_LEN` ####

Length of the preamble pattern in bits that is sufficient on most PHYs.

#### `SL_ADVANCED_BURST_DC_TRANSMITTER_ANCHOR_DELAY_US` ####

Anchor delay after the burst packets. Minimum is the rail state transition time
(minimum value is around 300).

#### `SL_ADVANCED_BURST_DC_TRANSMITTER_SCHEDULE_DELAY_US` ####

Measured delay of scheduled transmissions. I.e., it should be scheduled this
amount earlier

#### `SL_ADVANCED_BURST_DC_TRANSMITTER_BURST_LEN_MS` ####

The length of the wakeup (burst) transmission time.
