# RAIL Proprietary - Improved RF Sense Transmitter #

## Overview ##

This example application showcases how to enhance the RF energy sensing
capability of EFR32xG22 devices. It is a TX-only application that improves RF
Sense by transmitting packets across multiple frequencies. The value of this
example lies in demonstrating that the sensitivity of Selective RF Sense varies
with frequency. Additionally, it introduces the RF Sense OOK PHY through the
Radio Configurator, making it more accessible and convenient to use. While the
improvement approach was previously achievable with the frequency override debug
mode, integrating it as a user-defined PHY makes it more convenient to change
the frequency. To use this application, two nodes are required.

To get more information about this topic, read our [related
article](https://community.silabs.com/s/article/rfsense-on-efr32xg22?language=en_US).

> [!IMPORTANT]  
> This application lacks error handling and other critical features for
> readability. It is intended solely for educational purposes and is not
> recommended as a foundation for development.

## SDK version ##

SiSDK 2024.06.00 and above

## Hardware Required ##

EFR32xG22 and EFR32xG28 devices

## Connections Required ##

Connect an EFR32xG22 and an EFR32xG22/xG28 Development Kit to your PC.

### Tested boards for working with this example ###

| Board ID | Description  |
| ---------------------- | ------ |
| BRD4182A | [EFR32xG22 2.4 GHz 6 dBm Radio Board](https://www.silabs.com/development-tools/wireless/slwrb4182a-efr32xg22-wireless-gecko-radio-board?tab=overview) |
| BRD4401C | [EFR32xG28 868/915 MHz 20 dBm + 2.4 GHz 10 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg28-rb4401c-efr32xg28-2-4-ghz-ble-and-20-dbm-radio-board?tab=overview) |

Note, that this example requires LED and BTN functionalities, however no
EFR32xG27 board is available with this specification. For this reason, this
application is not usable with any of the boards using EFR32xG27. The correct
radio configuration can be found in the project folder, so it can be used with
RAILtest or in any custom application.

Note, that this RF Sense PHY is extremely sensitive in RX mode and it is not
intended to be used for that purpose, because detection happens very frequently
(RAILtest uses constant RX that should be disabled using this PHY).

## Setup ##

1. Generate Project with [Simplicity Studio
   v5](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#example-projects-demos-tab)
   or with [SLC
   CLI](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-tools-slc-cli/).
2. Adjust parameters detailed in the [Configuration](#configuration) section
   below.
3. Build and flash the project on the EFR32xG22/xG28 board.

## How It Works ##

This example application demonstrates how to improve the RF energy sensing
capability of EFR32xG22 devices. To use this application you need two nodes.

This program enables an EFR32xG22/xG28 device to transmit packets across
multiple frequencies using a predefined radio PHY. This radio PHY is the same,
that is configured by the related RF Sense RAIL API, but extended with more
channels having a small difference in frequency to the original channel. By
pressing the BTN0 button, the program initiates the transmission of multiple
packets through these channels with configurable delays between each
transmission. This approach improves the effective sensing range of the
EFR32xG22 device and surpasses the capabilities of standard RAIL API functions.
During the transmission, the currently used channel is displayed on the CLI.

> `SL_IMPROVED_RFSENSE_TRANSMITTER_PAYLOAD_LENGTH` is a variable used to define
> the payload length for transmission in the application. Its value should align
> with the payload length determined by the radio configuration when using a
> fixed-length configuration. Note, that the payload should always start with a
> 0x55 preamble byte, as the radio configuration contains 0 preamble bytes due
> to the used modulation restrictions. Additionally, it must not exceed the
> value of `SL_IMPROVED_RFSENSE_TRANSMITTER_FIFO_LENGTH`. The length of the Tx
> buffer should be set according to the radio configuration using this
> parameter. The `SL_IMPROVED_RFSENSE_TRANSMITTER_ARRAY_OF_CHANNELS` is a
> variable used to determine the array of channel indexes existing in the radio
> PHY which are used to transmit the packages on a button press.
> `SL_IMPROVED_RFSENSE_TRANSMITTER_TX_DELAY_US` is a parameter that sets the
> waiting period between these packets.

### How to use ###

To test the performance of this program, you need to flash a RAILtest example
app to an EFR32xG22 device. After this, use the `rfsense 2 0xB16F 1` CLI command
to start the RF sensing. The app will notify on a successful RF sensed event.

To initiate a packet transmission, you need to press the BTN0 button. During
transmission, LED0 will light up.

To modify the payload of the wake-up packet, make sure to always use the 0x55
byte first. The following bytes can be written in the buffer in little endian
byte order. The entire packet must be constructed in the application and written
to the Tx FIFO. Here is the correct setup for the above mentioned payload:

```c
SL_ALIGN(RAIL_FIFO_ALIGNMENT) static uint8_t tx_fifo[
  SL_IMPROVED_RFSENSE_TRANSMITTER_FIFO_LENGTH] = { 0x55, 0x6F, 0xB1 };
```

### Configuration ###

#### Project Specific Configurations ####

Additional project-specific configurations can be found in the
`<project_root>/config/sl_rail_improved_rfsense_transmitter_config.h` file.
These configurations are not currently accessible through the UI as component
settings, so you will need to modify them manually using a text editor or IDE.

#### `SL_IMPROVED_RFSENSE_TRANSMITTER_PAYLOAD_LENGTH` ####

Sets Payload length in bytes.

#### `SL_IMPROVED_RFSENSE_TRANSMITTER_FIFO_LENGTH` ####

Sets Tx and Rx FIFO length in bytes.

- [The size must be a power of 2 from 64 to
  4096](https://docs.silabs.com/rail/latest/rail-api/efr32-main#receive-and-transmit-fifo-buffers)

#### `SL_IMPROVED_RFSENSE_TRANSMITTER_ARRAY_OF_CHANNELS` ####

Array containing the channel indexes used to transmit the packets. These
channels must exist in the radio configuration as well.

#### `SL_IMPROVED_RFSENSE_TRANSMITTER_TX_DELAY_US` ####

Set the delay between the transmittions in us. Make sure to use a long enough
delay for the transmissions to finish.
