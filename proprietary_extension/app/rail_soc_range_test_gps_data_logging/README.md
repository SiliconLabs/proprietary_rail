# Proprietary Extension - SoC Range Test with GPS data logging #

## Overview ##

This application is a RAIL - SoC Range Test application with extended
functionalities. This extension include GPS positioning and measurement data
logging to an SD card using the [LEA-6S - GPS Click
(Mikroe)](https://www.mikroe.com/gps-click?srsltid=AfmBOoqk3wG-g_0UyxfOeI_jbcvnFF6dH9NyKIdq0GfwTrrmyYfzZSo8)
and [microSD Click
(Mikroe)](https://www.mikroe.com/microsd-click?srsltid=AfmBOoo_o28lCzM376nao_V0AQCOV_RXcyhvcBlvn6RwPngG6DDC4KyX)
boards.

Range Test applications are documented in [Flex SDK v3.x Range Test Demo User's
Guide](https://docs.silabs.com/rail/latest/flex-v3x-range-test-demo/). This
documentation only focuses on the functionality extension of the application.

## SDK version ##

SiSDK 2025.6.0 and above

## Hardware Required ##

EFR32 Series 2

## Connections Required ##

Connect two Development Kits to your PC.

### Tested boards for working with this example ###

| Board ID | Description                                                                                                                                                                           |
|----------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| BRD4182A | [EFR32MG22 2.4 GHz 6 dBm Radio Board](https://www.silabs.com/development-tools/wireless/slwrb4182a-efr32xg22-wireless-gecko-radio-board?tab=overview)                      |
| BRD4204D | [EFR32ZG23 868-915 MHz 14 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg23-rb4204d-efr32xg23-868-915-mhz-14-dbm-radio-board?tab=overview)                      |
| BRD4186C | [EFR32xG24 2.4 GHz 10 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg24-rb4186c-efr32xg24-wireless-gecko-radio-board?tab=overview)                              |

## Setup ##

1. Add RAIL Proprietary SDK Extension and Third Party Hardware Drivers
   extensions to the SDK you want to use.
2. Generate Project with [Simplicity Studio
   v5](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#example-projects-demos-tab)
   or with [SLC
   CLI](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-tools-slc-cli/).
3. Adjust parameters detailed in the [Configuration](#configuration) section
   below.
4. Build and flash the project on two boards.

## How It Works ##

The GPS and SD card modules must be connected to one of the devices used in the
Range Test. During operation, the device collects GPS data—including latitude,
longitude, altitude, and time—using the GPS module. These GPS parameters are
combined with the standard Range Test measurements and are logged to the SD card
via the SD card module. This allows for synchronized recording of both radio
performance and location data for later analysis.

### Logging control and Range Test states ###

Logging is managed by the **Range Test - SD card logging** component
(`range_test_sd_log`), not by the GPS or SD card drivers. Its
`app_process_action_range_test_log()` handler is registered by the component's
SLC event-handler contribution and is therefore called from the application's
internal process-action loop. The application drivers only implement the
operations requested by this component: detect the card, open or close the
file, write bytes, and retrieve the latest GPS data.

The component has its own private SD logging state machine:

| SD logging state | Action |
|------------------|--------|
| `WAIT_SD`        | Wait for an SD card to be inserted. |
| `IDLE`           | Wait for the base Range Test to begin a measurement. |
| `START`          | Open a CSV file and write the settings and column headers. |
| `LOGGING`        | Append a row whenever the Range Test measurement data changes. |
| `STOP`           | Close the CSV file and return to `IDLE`. |

The Range Test states enter the picture because the logging component observes
the existing global `state` owned by the Range Test application. They are
not logging states and the logging component does not change them. Instead,
they provide lifecycle boundaries for a measurement:

* In Rx mode, `START_MEASUREMENT` moves the logger from `IDLE` to `START`.
* In Tx mode, `SEND_MEASUREMENT` moves the logger from `IDLE` to `START`.
* In either mode, `MENU_SCREEN` moves the logger from `LOGGING` to `STOP`.

This relationship makes one CSV file correspond to one Range Test measurement
session. Removing the SD card from any state aborts the current log, attempts
to close the file, and returns the logger to `WAIT_SD`. A write failure moves
the logger to `STOP`; a file-open failure returns it to `IDLE`. When GPS support
is enabled, the latest GPS time is used for the file name and GPS values are
sampled for each logged measurement row.

### How to use ###

To use this example, connect the GPS and SD card modules to one of the
development boards. Currently, a limited set of development boards have the
required pin numbers and available peripherals to work out of the box. However,
the extensions can be ported to other custom boards that meet the requirements.

Below is a table showing how to connect the GPS and SD card modules to a
BRD4204D board with EFR32xG23:

| EFR32xG23 Pin | Module Connection | WSTK Breakout Pin |
|---------------|-------------------|-------------------|
| PD3           | GPS TXD           | F11               |
| PB2           | GPS RXD           | F10               |
| PD5           | GPS TMPLS         | F19               |
| Any GND Pin   | GPS GND           | Any GND Pin       |
| Any 3v3 Pin   | GPS 3.3V          | Any 3v3 Pin       |
| PD4           | GPS RESET         | F20               |
| Any GND Pin   | SD GND            | Any GND Pin       |
| Any 3v3 Pin   | SD 3.3V           | Any 3v3 Pin       |
| PC1           | SD SDI            | EXP_HEADER4       |
| PC2           | SD SDO            | EXP_HEADER6       |
| PC3           | SD SCK            | EXP_HEADER8       |
| PC0           | SD CS             | EXP_HEADER10      |
| PC5           | SD CD             | EXP_HEADER15      |

Adjust the connections as needed for your specific hardware setup.

### Configuration ###

#### Peripherals Used ####

The application extension utilizes an SPIDRV instance for the SD card
communication. Please note that with the current implementation, the LCD can be
on the same SPI bus as the SD card. An additional UART/EUSART instance is also
used for communicating with the GPS module. Please note that the current
implementation only allows the SD communication to work with the UART based
SPIDRV component, which leads to the limited number of compatible boards this
example supports out of the box.

#### Project Specific Configurations ####

Project-specific configurations can be found in the
`<project_root>/config/sl_gps_driver_config.h` file. These configurations are
accessible through the UI as component settings.

#### `SL_GPS_DRIVER_UART_RX_BUFFER_SIZE` ####

Sets the Rx buffer size in bytes.

#### `SL_GPS_DRIVER_PARSER_BUFFER_SIZE` ####

Sets the parser buffer size in bytes.

#### `SL_GPS_DRIVER_PARSER_RESULT_BUFFER_SIZE` ####

Sets the parser result buffer size in bytes.

#### `SL_GPS_DRIVER_PARSER_DATA_SIZE` ####

Sets the parser data size in bytes.

#### `SL_GPS_DRIVER_WAKEUP_DELAY_MS` ####

Sets the GPS wakeup time in milliseconds.

To further configure the project, please refer to the configuration of the
installed software components of the extensions.

## Notes - Known Issues ##

The SD card driver only supports SPIDRV software component, while SPIDRV EUSART
would be useful with many boards. For example the EFR32xG25 does not support the
SPIDRV software component (the EFR32xG28 also uses the EUSART version due to the
limited peripheral number). Please note that it is possible to port the driver
to use the mentioned SPIDRV EUSART component by modifying the required software
components from 'spidrv' to 'spidrv_eusart' under
[third_party_hw_drivers_extension/driver/component/services/mikroe_peripheral_drv_spi.slcc](https://github.com/SiliconLabsSoftware/third_party_hw_drivers_extension/blob/master/driver/component/services/mikroe_peripheral_drv_spi.slcc)

As the EFR32xG25 only has EUSART peripherals, the drv_uart driver of the
third_party_hw_drivers_extension also needs to be modified. This modified
[drv_uart.c](./src/drv_uart.c) file can be copied to replace the original file
under
<your_project>/third_party_hw_drivers_<version>/driver/pheripheral_drivers/mikroe/src/drv_uart.c

Due to the limited number of accessible pins on official boards with EFR32xG27,
this example cannot be used on these parts.

The packet sending period of the original Range Test example is too fast to
handle the GPS and logging tasks between packets, so it is necessary to increase
the `RANGE_TEST_SEND_TIME` define in
<your_sdk>/app/rail/component/sl_rail_sdk_range_test_core/app_measurement.c from
100000 to about 150000.
