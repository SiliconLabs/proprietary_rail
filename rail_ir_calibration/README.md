# RAIL Proprietary - Image Rejection Calibration #

## Overview ##

This example application demonstrates how to perform IR (Image Rejection)
calibration on EFR32 series 2 devices using the related RAIL APIs. The
application also demonstrates how to avoid calibrating if the application
reboots by loading calibration values from NVM. It can also be used to compare
memory requirements of these two solutions and see duration measurements.

To get more information about this topic, read our [related
article](https://docs.silabs.com/rail/latest/rail-training-calibration/#image-rejection-calibration).

> [!IMPORTANT]  
> This application lacks error handling and other critical features for
> readability. It is intended solely for educational purposes and is not
> recommended as a foundation for development.

## SDK version ##

SiSDK 2025.6.0 and above

## Hardware Required ##

EFR32 Series 2

## Connections Required ##

Connect an EFR32 series 2 development board to your PC.

### Tested boards for working with this example ###

| Board ID | Description                                                                                                                                                                           |
|----------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| BRD4182A | [EFR32xG22 2.4 GHz 6 dBm Radio Board](https://www.silabs.com/development-tools/wireless/slwrb4182a-efr32xg22-wireless-gecko-radio-board?tab=overview)                                 |
| BRD4204D | [EFR32ZG23 868-915 MHz 14 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg23-rb4204d-efr32xg23-868-915-mhz-14-dbm-radio-board?tab=overview)                      |
| BRD4186C | [EFR32xG24 2.4 GHz 10 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg24-rb4186c-efr32xg24-wireless-gecko-radio-board?tab=overview)                              |
| BRD4270B | [EFR32FG25 902-928 MHz 16 dBm Radio Board](https://www.silabs.com/development-tools/wireless/proprietary/fg25-rb4270b-efr32fg25-radio-board?tab=overview)                             |
| BRD4194A | [EFR32xG27 2.4 GHz 8 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg27-rb4194a-efr32xg27-8-dbm-wireless-radio-board?tab=overview)                               |
| BRD4401C | [EFR32xG28 868/915 MHz 20 dBm + 2.4 GHz 10 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg28-rb4401c-efr32xg28-2-4-ghz-ble-and-20-dbm-radio-board?tab=overview) |

## Setup ##

1. Generate the project using [Simplicity Studio
   v5](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#example-projects-demos-tab)
   or [SLC
   CLI](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-tools-slc-cli/).
2. Configure the necessary parameters as described in the
   [Configuration](#configuration) section.
3. Build and flash the project onto the board.

## How It Works ##

This application demonstrates how to handle IR calibration related tasks using
the RAIL API.

> The `SL_IR_CALIBRATION_DEFAULT_CONFIG_INDEX` and
> `SL_IR_CALIBRATION_DEFAULT_CHANNEL` parameters determine the index of the
> config and channel that is used for calibration (see available
> channels/configs in the radio_settings.radioconf file of the project). The
> `SL_IR_CALIBRATION_NUM_PROTOCOLS` should match the number of protocol
> configurations in the radio_settings.radioconf file.
> `SL_IR_CALIBRATION_ITERATE_ALL_PHYS` and `SL_IR_CALIBRATION_AUTO_IR_CAL` can
> be used to automate the calibration process. The application also showcases
> the use of NVM memory for saving and loading IR calibration values, where the
> base key for the storing is set with `SL_IR_CALIBRATION_NVM3_KEY_BASE`.
> `SL_IR_CALIBRATION_ENABLE_PERFORM_IR_CAL` can be used to enable the
> calibration API of RAIL, so the application size can be compared between
> calibrating in the application or applying known values.

### How to use ###

#### CLI commands ####

- help: List the available CLI commands.
- perform_ir_cal: Run sl_rail_calibrate_ir() on the current channel/config. With
  `iterate_all_phys` on, walks every channelConfigs[] entry.
- get_pending_ir_cal: Get the pending IR calibration status for the current
  channel/config.
- apply_ir_cal_values: Apply the given IR calibration values for the current
  config via sl_rail_apply_ir_calibration().\
*[uint32opt] (rx_ir0) (rx_ir1) ((dc_offset_iq) (phi_epsilon) on FG25 OFDM-PA)*
- get_ir_cal_values: Get the active IR calibration values for the current config.
- save_ir_cal_values: Persist the current config's IR calibration values to NVM3.
- load_ir_cal_values: Load the current config's IR calibration values from NVM3
  and apply them. The reported duration includes the NVM3 lookup/read so the
  production cost is visible.
- status: Print state, per-config slots and last-measured timings.
- set_channel: Set the active channel within the current radio config.\
*[uint16] channel index*
- get_channel: Get the active channel within the current radio config.
- set_configindex: Switch to channelConfigs[index] via
  sl_rail_config_channels().\
*[uint8] config index*
- get_configindex: Print the active config index.
- set_auto_ir_cal: When 1: CAL_NEEDED auto-runs sl_rail_calibrate_ir(). When 0:
  the event is logged but deferred until the user invokes `perform_ir_cal`.\
*[uint8] [0=Disable] 1=Enable*
- get_auto_ir_cal: Print the active auto_ir_cal setting.
- set_iterate_all_phys: When 1: `perform_ir_cal` walks every channelConfigs[]
  entry (switching via sl_rail_config_channels) and calibrates each slot.\
*[uint8] [0=Disable] 1=Enable*
- get_iterate_all_phys: Print the active iterate_all_phys setting.
- get_rssi: Get RSSI on the active channel (image-frequency test helper).
- set_rf_path: Set the RF path.\
*[uint8] [0=ANT0] 1=ANT1*
- get_rf_path: Get the RF path.

#### Testing ####

Functionality examples on EFR32xG25:

On startup the following message will be displayed:

```
> IR calibration example
> Protocols compiled in   : 2
> Active config index     : 0
> Active channel          : 0
> auto_ir_cal             : off
> iterate_all_phys        : off
> perform_ir_cal linked   : yes
> {{(ir_cal)}{CAL_NEEDED: OFDM_TX_IR_CAL RX_IR_CAL} on {cfg:0}{ch:0} (deferred - run `perform_ir_cal`)}
```

##### Performing calibration #####

Issue the `perform_ir_cal` command to calibrate the active config:

```
perform_ir_cal
> {{(ir_cal)}{cfg:0}{rf_path:0}{duration:100424 us}}
> {{(ir_cal)}{cfg:0}{rx_ir:(0x80A5809C;0xFFFFFFFF)}{tx_ir:(dc=0xFFFFFFFF;phi=0xFFFFFFFF)}}
```

Issue the `set_configindex 1` command to modify the active config to
the second one (if exists):

```
set_configindex 1
> {{(set_configindex)}{active config:1}}
> {{(set_configindex)}{the active channel is changed to:20480}}
> {{(ir_cal)}{CAL_NEEDED: RX_IR_CAL} on {cfg:1}{ch:20480} (deferred - run `perform_ir_cal`)}
```

Issue the `set_auto_ir_cal 1` command to enable automatic IR
calibration on request:

```
set_auto_ir_cal 1
> {{(set_auto_ir_cal)}{auto_ir_cal:on}}
> {{(set_auto_ir_cal)}{servicing pending IR calibration request now:}}
> {{(ir_cal)}{cfg:1}{rf_path:0}{duration:100481 us}}}
> {{(ir_cal)}{cfg:1}{rx_ir:(0x809D80D9;0xFFFFFFFF)}{tx_ir:(dc=0x03EB0016;phi=0x0FA80013)}}
```

##### Loading/saving calibration values #####

Issue the `save_ir_cal_values` command to save the values to NVM
memory:

```
save_ir_cal_values
> {{(save_ir_cal_values)}{saved cfg to NVM3:1}}
```

Reset the device.

Issue the `set_configindex 1` command to change config then issue the
`load_ir_cal_values` command to load the values from the NVM memory:

```
load_ir_cal_values
> {{(load)}{cfg:1}{key:0x0000BEE1}{duration:46 us (incl. NVM3 read)}}
```

You can compare the time it takes to perform the calibration and to load it from
NVM memory.

Custom values can also be applied with the `apply_ir_cal_values` command: Note,
that at least 2 values need to be passed (for both RF Paths), however on
EFR32xG25 4 values are also supported for setting the OFDM calibration values as
well.

##### Applying custom calibration values #####

```
apply_ir_cal_values 0xFF4CFEF7 0xFFFFFFFF 0x03F2000C 0x0FCA0FF0
> {{(apply)}{cfg:1}{rf_path:0}{duration:4 us}}
```

##### Status of the calibrations #####

Issue the `status` command to get all the calibration and timing information
from the application.

```
status
> {{(status)}{current_cfg:1}{current_ch:20480}{auto_ir_cal:off}{iterate_all_phys:off}{ir_cal_pending:no}
> {cfg:0 UNCAL}{cal:0 us}{apply:0 us}{load:0 us}
> {rx_ir:{0xFFFFFFFF;0xFFFFFFFF}}{tx_ir:{dc=0xFFFFFFFF;phi=0xFFFFFFFF}}
> {cfg:1 CAL(NVM)}{cal:0 us}{apply:45 us}{load:45 us}
> {rx_ir:{0xFF22FEFD;0xFFFFFFFE}}{tx_ir:{dc=0x03F2000C;phi=0x0FCA0FF0}}
```

##### Demonstrating the calibration effect #####

To test the effect of the IR calibration you will need another device:

1. Build and flash the RAILTest application on it with the radio configuration
   that you want to test.

2. Issue the following CLI command to transmit a carrier wave on the image
   frequency in the RAILTest application:

   ```
   rx 0
   setDebugMode 1
   freqOverride <frequency>
   setTxTone 1
   ```

   Where frequency = Actual Carrier Frequency [Hz] +/- 2 * Actual Intermediate
   Frequency [Hz] (based on Injection Side). These parameters are available in
   the autogen/radioconf_generation_log.json file.

3. Issue the following CLI command to measure the RSSI in the IR calibration
   example application:

   ```
   get_rssi
   > {{(get_rssi)}{rssi:-112}}
   ```

##### Most common calibration procedure #####

The best practice in an application is to perform the calibration on all the
used configs at startup. This can be done with the following commands:

```
set_iterate_all_phys 1
> {{(set_iterate_all_phys)}{iterate_all_phys:on}}
set_auto_ir_cal 1
> {{(set_auto_ir_cal)}{auto_ir_cal:on}}
```

##### Comparing code sizes #####

The `SL_IR_CALIBRATION_ENABLE_PERFORM_IR_CAL` macro can be used to check the
code size differences between using the RAIL calibration or applying saved
values from the NVM memory. In this example, the size of the .bin file is
187,884 bytes, but with disabled macro it is only 182,700 bytes.

##### Change RF Path #####

Separate RF Paths are also supported by this example. To configure the RF Path
on a supported device, the `set_rf_path` command can be used (use `get_rf_path` to
get the active path). This can also be configured on these supported devices at
build time by the `SL_RAIL_UTIL_RF_PATH_INT_RF_PATH_MODE` macro.

### Configuration ###

#### Peripherals Used ####

The application utilizes the default EUSART instance for CLI communication and
console logging. The VCOM enable signal is pre-configured for the tested boards.

You may want to adjust the EUSART (VCOM) configurations according to the
board you are using.

NVM3 is also used to save and load calibration data.

Additionally, it uses 1 PRS channel to access the `PRS_RACL_RX` signal. It is
available at the `EXP_HEADER_15` debug pin on Wireless Motherboards. As the
radio is in Rx state during calibration, the duration can be measured using this
signal as well.

#### Project Specific Configurations ####

Additional project-specific configurations can be found in the
`<project_root>/config/sl_rail_ir_calibration_config.h` file. These
configurations are not currently accessible through the UI as component
settings, so you will need to modify them manually using a text editor or IDE.

#### `SL_IR_CALIBRATION_DEFAULT_CONFIG_INDEX` ####

Sets the index of the default config used to calibrate.

#### `SL_IR_CALIBRATION_DEFAULT_CHANNEL` ####

Sets the index of the default channel used to calibrate.

#### `SL_IR_CALIBRATION_NUM_PROTOCOLS` ####

Sets the number of protocols that is present in the radio configuration. One per
band you intend to use.

#### `SL_IR_CALIBRATION_ITERATE_ALL_PHYS` ####

Boot value of the global `iterate_all_phys` flag. When true, `perform_ir_cal`
walks every channelConfigs[] entry, switches into it, and runs the cal. When
false, only the currently active PHY is calibrated. The flag is mutable at
runtime (`set_iterate_all_phys 0|1`).

#### `SL_IR_CALIBRATION_ENABLE_PERFORM_IR_CAL` ####

This switch brackets the RAIL IR_CAL entry point so the linker can dead-strip the
calibration engine when it is disabled. The point of the demo is to contrast
"compute fresh values on the device" with "load them from NVM3 and apply", so
only the calibrate-side calls are gated. The sl_rail_apply_ir_calibration() /
NVM3 paths are always linked.

#### `SL_IR_CALIBRATION_AUTO_IR_CAL` ####

Default value of the global `auto_ir_cal` flag. When true,
SL_RAIL_EVENT_CAL_NEEDED runs sl_rail_calibrate_ir() right away, the user is
only notified. When false, CAL_NEEDED is logged but deferred. The user runs
`perform_ir_cal` (or presses the trigger button) when they are ready. False is
what makes the before/after RSSI comparison meaningful.

#### `SL_IR_CALIBRATION_NVM3_KEY_BASE` ####

Set the base NVM3 key for storing calibration data. Each protocol slot uses
`SL_IR_CALIBRATION_NVM3_KEY_BASE + index`

## Notes - Known Issues SiSDK 2025.06.02 ##

- SL_RAIL_CAL_OFDM_TX_IR_CAL can't be performed separately from
  SL_RAIL_CAL_RX_IR_CAL. Always use SL_RAIL_CAL_ONETIME_IR_CAL with the
  sl_rail_calibrate() function for IR calibration.
- SL_RAIL_CAL_OFDM_TX_IR_CAL calibration is requested on non-OFDM PHY (EFR32xG25).
- Pending IR calibrations stay active after config change. When an IR
  calibration is only requested, this request is not cleared after a protocol
  change.
- It is required to put the different frequency band PHYs to different protocol
  configurations in the radio configuration, because the IR calibration can only
  be performed on different protocol configs.
