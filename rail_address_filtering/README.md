# RAIL Proprietary - Address Filtering #

## Overview ##

This example application demonstrates how to use address filtering with RAIL.

To get more information about this topic, read our API documentation [related
API](https://docs.silabs.com/rail/latest/rail-api/address-filtering).

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

This example application demonstrates how to use the address filtering
capabilities of RAIL. To use this application you need two nodes.

The packet sent event is indicated with LED0 on the boards. If a packet is
successfully received, LED1 will toggle to indicate this event.

This program provides detailed information via the terminal, including currently
active addresses, the success or filtering status of received packets and user
instructions for operating the application.

> `SL_ADDRESS_FILTERING_PAYLOAD_LENGTH` is a variable used to define the payload
> length for transmission in the application. Its value should align with the
> payload length determined by the radio configuration when using a fixed-length
> configuration. Additionally, it must not exceed the value of
> `SL_ADDRESS_FILTERING_BUFFER_LENGTH`. The length of the Tx buffer should be
> set according to the radio configuration using this parameter. The
> `SL_ADDRESS_FILTERING_DEFAULT_CHANNEL` parameter determines the index of the
> channel that is used for packet transmission and reception (see available
> channels in the radio_settings.radioconf file of the project). With the
> `SL_ADDRESS_FILTERING_FIELD_ZERO_ADDRESSES` and
> `SL_ADDRESS_FILTERING_FIELD_ONE_ADDRESSES` arrays, the available address
> values can be determined, while the `SL_ADDRESS_FILTERING_MATCH_TABLE`
> variable is used to define the correct adresses. You can find more information
> on this table in the API.

Note that this example supports only 1-byte long addresses positioned at the
start of the payload. Additionally, predefined match tables are available for
single and double field filtering for simple configurations. For more details,
refer to the API documentation.

### How to use ###

To initiate a packet transmission, you need to press the BTN0 button. With BTN1,
you can cycle through the predefined addresses to be present in the sent packet.
Note that the default radio state in this application is Rx state. It goes back
to this state after each Rx or Tx success/failure, so it is always capable of
receiving from the other node, and this behavior is symmetric. Additionally, the
calibration runs before the radio's first use to ensure optimal performance.

### Configuration ###

#### Project Specific Configurations ####

Additional project-specific configurations can be found in the
`<project_root>/config/sl_rail_address_filtering_config.h` file. These
configurations are not currently accessible through the UI as component
settings, so you will need to modify them manually using a text editor or IDE.

#### `SL_ADDRESS_FILTERING_PAYLOAD_LENGTH` ####

Sets Payload length in bytes.

#### `SL_ADDRESS_FILTERING_BUFFER_LENGTH` ####

Sets Tx and Rx Buffer length in bytes.

- [The size must be a power of 2 from 64 to
  4096](https://docs.silabs.com/rail/latest/rail-api/efr32-main#receive-and-transmit-fifo-buffers)

#### `SL_ADDRESS_FILTERING_DEFAULT_CHANNEL` ####

Set the index of the default channel used to transmit and receive.

- Only configured channel index can be used (see available channels in the
  radio_settings.radioconf file of the project).

#### `SL_ADDRESS_FILTERING_FIELD_ZERO_ADDRESSES` ####

Set the address values of Field0. It also contains a don't care value.

#### `SL_ADDRESS_FILTERING_FIELD_ONE_ADDRESSES` ####

Set the address values of Field1. It also contains a don't care value.

#### `SL_ADDRESS_FILTERING_MATCH_TABLE` ####

Set the match table for address filtering

|            | No Match | Address 0 | Address 1 | Address 2 | Address 3 |
|------------|----------|-----------|-----------|-----------|-----------|
| **No Match** | bit 0   | bit 1     | bit 2     | bit 3     | bit 4     |
| **Address 0** | bit 5   | bit 6     | bit 7     | bit 8     | bit 9     |
| **Address 1** | bit 10  | bit 11    | bit 12    | bit 13    | bit 14    |
| **Address 2** | bit 15  | bit 16    | bit 17    | bit 18    | bit 19    |
| **Address 3** | bit 20  | bit 21    | bit 22    | bit 23    | bit 24    |

Note, that a [python
program](../tools/python_address_filtering/match_table_generator.py) is
available to generate the necessary variable from a given table. This program
can also be used to create the table from an existing variable. To use this
program, the tkinter python package needs to be installed.
