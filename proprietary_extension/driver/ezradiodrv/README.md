# EZRADIODRV #

This directory contains the legacy EZRadio / EZRadioPRO driver implementation
packaged as part of the `proprietary_extension` SDK extension.

The driver stack provides multiple access layers around an external EZRadio
device connected to a Silicon Labs SoC host, including:

- Radio HAL
- Communication layer
- Radio API layer
- Plug-in manager and optional plug-ins

The extension currently provides the following related components:

| No  | Component    | Role                                          |
| --- | ------------ | --------------------------------------------- |
| 1   | `ezradiodrv` | Main driver component                         |
| 2   | `si4455`     | EZRadio part component                        |
| 3   | `si4460`     | EZRadioPRO part component                     |
| 4   | `si4461`     | EZRadioPRO part component                     |
| 5   | `si4463`     | EZRadioPRO part component                     |
| 6   | `si4468`     | EZRadioPRO part component for Si4467 / Si4468 |

## Usage ##

The normal usage flow is:

1. Start from the included `ezradio_empty` example or from a new SoC project.
2. Add the `ezradiodrv` component.
3. Add the radio part component that matches your external device.
4. Replace the project `radio_config.h` with a WDS-generated configuration for
   your target radio and PHY.
5. Review `sl_spidrv_exp_config.h` and the EZRADIODRV GPIO settings to match
   your hardware wiring.
6. Enable the optional plug-ins your application needs.

In Simplicity Studio, the manual component flow is:

1. Open the `.slcp` file in your project.
2. Go to the `SOFTWARE COMPONENTS` tab.
3. Enable the `Experimental` quality filter.
4. Select the appropriate component under `Driver/EZRadio`.

## Component Requirements ##

The main `ezradiodrv` component requires:

- `spidrv`
- `gpiointerrupt`
- `ustimer`
- `ecode`

It also recommends a `spidrv` instance named `exp`.

One of the supported radio part components must also be selected.

## Configuration ##

Important configuration expectations:

- Radio settings are expected to be generated outside Simplicity Studio with
  Wireless Development Suite (WDS).
- The generated radio configuration should be added to the project as
  `radio_config.h`.
- If the selected radio configuration requires a patch file, add that patch to
  the project as well.
- SPI settings must be reviewed in `sl_spidrv_exp_config.h`.
- EZRADIODRV communication and control GPIO settings must match the target
  hardware wiring.
