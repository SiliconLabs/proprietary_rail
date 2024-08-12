# RAIL - OTA DFU Target

![Type badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_target_common.json&label=Type&query=type&color=green)
![Technology badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_target_common.json&label=Technology&query=technology&color=green)
![License badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_target_common.json&label=License&query=license&color=green)
![SDK badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_target_common.json&label=SDK&query=sdk&color=green)
![Build badge](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_target_build_status.json)
![Flash badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_target_common.json&label=Flash&query=flash&color=blue)
![RAM badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_ota_dfu_target_common.json&label=RAM&query=ram&color=blue)
## Overview

The OTA DFU Target Example application demonstrates how an application image can be received and passed to the bootloader for OTA DFU, using basic RAIL operation.

## Gecko SDK Version

Simplicity SDK Suite v2024.6.1

## Hardware Required

The OTA DFU Target application is supported by every EFR32 generations and families that support proprietary software. Two LEDs are used for a simple UI but not required for the core operation. A Wireless Starter Kit (WSTK) has everything this example needs.

## Connections Required

The USB port of the WSTK is connected to a PC for programming and logging.

## Setup

A bootloader that supports at least one storage slot is required. This application is tested with `Bootloader - SoC Internal Storage`.

The application is based on the `RAIL - Soc Empty` example that's included in the SDK. Here are the steps needed to modify the project:

- Create a project based on the `RAIL - Soc Empty` that's configured for your radio board.
- Add the `Bootloader Application Interface` software component.
- Add the `Simple LED` software component and an additional LED instance (led1).
- Add the `Sleep Timer` software component.
- Add the `Log` software component based on `IO Stream: USART Core` or `IO Stream: EUSART Core`.
- Add the `IO Stream: EUSART` or `IO Stream: USART` software component according to the core selected for `Log`.
- In the `Board Control` software component, turn on `Enable Virtual COM UART`.
- Copy all the files in the repositry to the project folder.
- In the `RAIL configurator`, turn on `Customized` in `General Settings` and set the `Frame Fixed Length` to 20 bytes.

Compile the project and download the application to your radio board.

## How It Works

- The radio starts in receive state, waiting for a `start` packet that indicates size of GBL file which contains the updated application image.
- The file size is compared to the size of the storage slot reserved by the bootloader. If it can fit in the slot, a `go head` response packet is sent to the host. Otherwise a `no space` response will be sent, and the OTA DFU process is halted.
- The host then starts sending `data` packets, which contain a fragment of the image, with an 28-bit offset and 16-byte data. That data is written to the storage slot. A `data received` response is sent to the host. This step is repeated until all the fragments are received.
- If the received fragment address is not identical to the next expected address, a `data received` response is sent to the host with the last received address. If any error is detected, the `data error` response is sent instead also with the last received address.
- When the last fragment is received, the `finished` response is sent to the host. After a 3-second delay, the bootloader is invoked to performe DFU.
- If any error happens, a reset is used to start the whole process. 

The application uses the LEDs to indicate the status of the OTA DFU.
- After reset, LED 0 blinks twice every second if the radio is in receive state. LED 1 blinks
  once second if any error happens in start-up. You may want to change the blink frequency or duty
  cycle when creating an update image, so a successful update can be easily recognized.
- LED 0 blinks once every second when the DFU image is being received.
- LED 1 blinks once every second if the OTA DFU process is not successful.
- LED 0 stays on for three seconds when the whole image is received. After three seconds, 
  the LED is turned off and the bootloader is invoked.

An example `RAIL - OTA DFU Host` is available as a host reference design.

## Notes

For simplicity, this example assumes that no other devices are communicating with the same protocol. To enable concurrent operation, some session ID or device ID in the packet should be added in the packet. HW and version checks, and security features such as authentication and encryption are possible additions to this simple protocol.

## Resources

- [RAIL Tutorial
  Series](https://community.silabs.com/s/article/rail-tutorial-series?language=en_US): it is advised to read through the `Studio v5 series` first to familiarize the basics of packet transmission and reception.

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via [Silicon Labs Community](https://community.silabs.com/s/topic/0TO1M000000qHaKWAU/proprietary?language=en_US).
