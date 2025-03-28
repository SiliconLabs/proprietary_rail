# RAIL Proprietary - Tutorial Downloading Messages #

## Overview ##

This example application demonstrates calibration event handling and downloading
packets in RAIL.

To get more information about this topic, read our [related
article](https://docs.silabs.com/rail/latest/rail-training-trx/).

> [!IMPORTANT]  
> This application lacks error handling and other critical features for
> readability. It is intended solely for educational purposes and is not
> recommended as a foundation for development.

## SDK version ##

SiSDK 2024.6.0 and above

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
| BRD4401C | [EFR32xG28 868/915 MHz 20 dBm + 2.4 GHz 10 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg28-rb4401c-efr32xg28-2-4-ghz-ble-and-20-dbm-radio-board?tab=overview) |

## Setup ##

1. Generate Project with [Simplicity Studio
   v5](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#example-projects-demos-tab)
   or with [SLC
   CLI](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-tools-slc-cli/).
2. Adjust parameters detailed in the [Configuration](#configuration) section
   below.
3. Build and flash the project on two boards.

## How It Works ##

This example application demonstrates calibration event handling and downloading
packets in RAIL as well as it has packet transmission capabilities. To use this
application you need two nodes.

The packet sent and received events are indicated with LED0 on the boards. If an
error occures during the transmission or reception of the packets, LED1 is used
to indicate this.

If a packet is received by one of the devices, it prints its payload and RSSI (
received signal strength indicator) value to the terminal.

> `SL_TUTORIAL_DOWNLOADING_MESSAGES_PAYLOAD_LENGTH` is a variable used to define
> the payload length for transmission in the application. Its value should align
> with the payload length determined by the radio configuration when using a
> fixed-length configuration. Additionally, it must not exceed the value of
> `SL_TUTORIAL_DOWNLOADING_MESSAGES_BUFFER_LENGTH`. The length of the Tx buffer
> should be set according to the radio configuration using this parameter. The
> `SL_TUTORIAL_DOWNLOADING_MESSAGES_DEFAULT_CHANNEL` parameter determines the
> index of the channel that is used for packet transmission and reception (see
> available channels in the radio_settings.radioconf file of the project).

### How to use ###

To initiate a packet transmission, you need to press the PB0 button. Note that
the default Radio state in this application is Rx state. It goes back to this
state after each Rx or Tx success/failure, so it is always capable of receiving
from the other node, and this behavior is symmetric. Additionally, the
calibration runs before the radio's first use to ensure optimal performance.

### Configuration ###

#### Peripherals Used ####

The application utilizes the default EUSART instance for console logging. The
VCOM enable signal is pre-configured for the tested boards.

You may want to adjust the EUSART (VCOM) configurations according to the
board you are using.

#### Project Specific Configurations ####

Additional project-specific configurations can be found in the
`<project_root>/config/sl_rail_tutorial_downloading_messages_config.h` file.
These configurations are not currently accessible through the UI as component
settings, so you will need to modify them manually using a text editor or IDE.

#### `SL_TUTORIAL_DOWNLOADING_MESSAGES_PAYLOAD_LENGTH` ####

Sets Payload length in bytes.

#### `SL_TUTORIAL_DOWNLOADING_MESSAGES_BUFFER_LENGTH` ####

Sets Tx and Rx Buffer length in bytes.

- [The size must be a power of 2 from 64 to
  4096](https://docs.silabs.com/rail/latest/rail-api/efr32-main#receive-and-transmit-fifo-buffers)

#### `SL_TUTORIAL_DOWNLOADING_MESSAGES_DEFAULT_CHANNEL` ####

Set the index of the default channel used to transmit and receive.

- Only configured channel index can be used (see available channels in the
  radio_settings.radioconf file of the project).
  