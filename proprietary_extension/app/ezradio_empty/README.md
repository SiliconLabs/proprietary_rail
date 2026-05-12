# Proprietary Extension - EZRadio Driver - Empty #

## Overview ##

This example is an empty starting point for projects built around the legacy
EZRADIODRV driver and EZRadio devices. It is included in this repository as
part of the `proprietary_extension` SDK extension and is intended to
demonstrate project creation with the old, discontinued EZRadio driver flow.

This is **not** a RAIL example. The project uses the extension-provided
`ezradiodrv` and `si4455` components instead of the EFR32 built-in RAIL radio.

The application itself is intentionally minimal:

- `app_init()` prints a startup message and calls `ezradioInit()`.
- `app_process_action()` is empty.
- No transmit, receive, or other EZRadio plug-ins are enabled by default.

Use this project as a clean base for adding your own driver plug-ins,
application state machine, radio operations, and board-specific integration.

## Dependencies ##

- Simplicity SDK with the `proprietary_extension` installed and trusted.
- The extension-provided `ezradiodrv` component.
- A supported EZRadio device component. This example is configured for `si4455`.
- A WDS-generated radio configuration header. The project currently includes
  `config/radio_config.h` generated for `Si4455 Rev. B1`.

The project metadata in
`SimplicityStudio/ezradio_empty.slcp` declares `proprietary_extension` version
`1.0.0` and pulls in the following main software components:

- `sl_main`
- `device_init`
- `app_log`
- `iostream_retarget_stdio`
- `iostream_recommended_stream`
- `clock_manager`
- `si4455`
- `ezradiodrv`

## Hardware Required ##

- One supported EFR32-based host board.
- One EZRadio(PRO) transceiver connected to the host through SPI and the
  required control pins.

This example does not use the on-chip EFR32 radio path. It is intended for an
external EZRadio(PRO) device controlled by the EZRADIODRV HAL.

### Tested Boards for This Example ###

| Board ID | Description                                                                                                                                                      |
| -------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| BRD4204D | [EFR32ZG23 868-915 MHz 14 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg23-rb4204d-efr32xg23-868-915-mhz-14-dbm-radio-board?tab=overview) |

## Connections Required ##

The EZRADIODRV configuration expects:

- One SPI instance for radio communication through the `SPIDRV exp` instance.
- The radio interrupt line: `RF_INT`
- The radio shutdown line: `RF_SDN`
- Optional/driver-used GPIO lines: `RF_GPIO0` and `RF_GPIO1`
- Chip select routed from the `SPIDRV exp` configuration

Also review the `sl_spidrv_exp_config.h` settings in the generated project and
the EZRADIODRV pin definitions when adapting the example to a different board.

## Setup ##

1. Install the `proprietary_extension` into your Simplicity SDK extension area
  and trust it with `slc` if required by your workflow.
2. Create the project from the `ezradio_empty` example in Simplicity Studio or
   generate it with `slc`.
3. Verify the selected EZRadio part component matches your hardware. This
   example is configured for `Si4455`.
4. Review the SPI configuration for the radio interface in the generated
   `sl_spidrv_exp_config.h` file.
5. Review the EZRADIODRV GPIO and communication settings before first bring-up.
6. Replace or update `config/radio_config.h` if your radio settings differ from
   the provided WDS-generated configuration.
7. Build and flash the project.
8. Open a VCOM terminal or Simplicity Studio console to observe the startup log.

## How It Works ##

The application keeps the implementation minimal.

During initialization, `app_init()` prints:

```text
Proprietary Extension - EZRadio Driver - Empty application
```

After logging the banner, the application calls `ezradioInit()` to initialize
the EZRadio driver stack and apply the configuration from `radio_config.h`.

No application logic runs after initialization. The example serves as a project
skeleton rather than a complete radio demo.

## How to Use ##

1. Generate, build, and flash the project.
2. Power the host board and attached EZRadio hardware.
3. Open the console output.
4. Confirm that the startup banner appears.
5. Use the project as a base for your own feature development.

Typical next steps are enabling one or more EZRadio plug-ins, adding packet
handling code, or replacing the default radio configuration with one generated
for your target PHY.

## Configuration ##

### Peripherals Used ###

- Default application log / stdio stream for console output
- `SPIDRV exp` instance for radio SPI communication
- GPIO control lines defined for the EZRadio HAL:
  - `RF_INT`
  - `RF_SDN`
  - `RF_GPIO0`
  - `RF_GPIO1`

### Project Specific Configurations ###

The most important project-specific configuration surfaces are:

- `config/radio_config.h`
- EZRADIODRV component configuration in the generated project
- `sl_spidrv_exp_config.h` in the generated project

### `config/radio_config.h` ###

Replace this file with a WDS-generated radio configuration that matches your
target requirements. Simplicity Studio is not suitable for this kind of radio
configuration generation.

### EZRADIODRV Settings ###

The EZRADIODRV component configuration controls:

- Optional plug-in enablement
- CTS polling behavior
- SPI mode selection
- EZRadio control GPIO assignment

In the default configuration used by this example, all optional plug-ins are
disabled, which keeps the project focused on bare initialization only.

## Notes ##

- This example exists to demonstrate project creation and bring-up with the
  legacy EZRADIODRV flow now redeployed as an extension in this repository.
- It is only loosely related to the rest of the proprietary examples in this
  repository and should not be interpreted as a RAIL-based project.
- The code is marked experimental in the project metadata and source headers.
- Exact tested radio-side wiring is intentionally left open here for board-
  specific completion.
