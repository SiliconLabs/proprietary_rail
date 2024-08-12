# RAIL - OTA DFU Host

![Type badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_host_common.json&label=Type&query=type&color=green)
![Technology badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_host_common.json&label=Technology&query=technology&color=green)
![License badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_host_common.json&label=License&query=license&color=green)
![SDK badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_host_common.json&label=SDK&query=sdk&color=green)
![Build badge](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_host_build_status.json)
![Flash badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_host_common.json&label=Flash&query=flash&color=blue)
![RAM badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_host_common.json&label=RAM&query=ram&color=blue)
## Overview

The OTA DFU Host application is an example that demostrates a simple OTA DFU process that is implemented in the `RAIL - OTA DFU Target` application. 

## Gecko SDK Version

Simplicity SDK Suite v2024.6.1

## Hardware Required

The OTA DFU Host application is supported by every EFR32 generations and families that support proprietary software. Two LEDs and one button are used for a simple UI but not required for the core operation. A Wireless Starter Kit (WSTK) has everything this example needs.

## Connections Required

The USB port of the WSTK is connected to a PC for programming and logging.

## Setup

A bootloader that supports at least one storage slot is required. This application is tested with `Bootloader - SoC Internal Storage`.

The application is based on the `RAIL - Soc Empty` example that's included in the SDK. Here are the steps needed to modify the project:

- Create a project based on the `RAIL - Soc Empty` that's configured for your radio board.
- Add the `Bootloader Application Interface` software component.
- Add the `Simple LED` software component and an additional LED instance (led1).
- Add the `Simple Button` software component.
- Add the `Sleep Timer` software component.
- Add the `Log` software component based on `IO Stream: USART Core` or `IO Stream: EUSART Core`.
- Add the `IO Stream: EUSART` or `IO Stream: USART` software component according to the core selected for `Log`.
- In the `Board Control` software component, turn on `Enable Virtual COM UART`.
- Copy all the files in the repositry to the project folder.
- In the `RAIL configurator`, turn on `Customized` in `General Settings` and set the `Frame Fixed Length` to 20 bytes.

Compile the project and download the application to your radio board.

## How It Works

The GBL file created from the target project needs to be flashed to the bootloader storage slot using the `Flash Programer` in binary mode. The slot address is printed through Log in the function app_init. Note that the extension of the GBL file should be changed to `.bin` so that the Flash Programmer will copy the content to the flash without any further actions.

The application uses the LEDs to indicate the status of the OTA DFU.
- After reset, LED 0 is lit to indicate a valid DFU image is found. LED 1 blinks once second if any error happens in start-up. 
- When button 0 is pressed, the OTA DFU process is started.
- LED 0 blinks once every second when the DFU image is being transferred.
- LED 1 blinks once every second if the OTA DFU process is not successful. A reset is needed to start the process again.
- LED 0 stays on after the whole image is transferred. The OTA DFU process can be started again by pressing button 0.

## Resources

- [RAIL Tutorial
  Series](https://community.silabs.com/s/article/rail-tutorial-series?language=en_US):
  it is advised to read through the `Studio v5 series` first to familiarize the
  basics of packet transmission and reception.

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via
[Silicon Labs
Community](https://community.silabs.com/s/topic/0TO1M000000qHaKWAU/proprietary?language=en_US).
