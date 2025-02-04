# RAIL Proprietary - Direct to Buffer #

## Overview ##

This example application demonstrates how to use Direct-to-Buffer (
[`RAIL_RxDataSource_t`](https://docs.silabs.com/rail/latest/rail-api/data-management#rail-rx-data-source-t))
modes.

The application sets the radio to receive mode and continuously captures the
selected specialized data source into a buffer. Upon user request, triggered by
pressing the pushbutton on the development kit, it can output the most recent
segments of the captured data to the console.

Additionally, this project traces out Direct-to-PRS signals where available.

For more details on Rx Direct Mode, refer to the [RAIL
documentation](https://docs.silabs.com/rail/latest/rail-developers-guide-direct-mode/).

## SDK version ##

SiSDK 2024.6.2 and above

## Hardware Required ##

EFR32 Series 2

## Connections Required ##

Connect a Development Kit to your PC. Additionally, you may want to connect a
logic analyzer to the debug pins and/or open a serial terminal connected to the
VCOM port.

### Tested boards for working with this example ###

| Board ID | Description                                                                                                                                                                           |
|----------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| BRD4182A | [EFR32xG22 2.4 GHz 6 dBm Radio Board](https://www.silabs.com/development-tools/wireless/slwrb4182a-efr32xg22-wireless-gecko-radio-board?tab=overview)                                 |
| BRD4204D | [EFR32ZG23 868-915 MHz 14 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg23-rb4204d-efr32xg23-868-915-mhz-14-dbm-radio-board?tab=overview)                      |
| BRD4186C | [EFR32xG24 2.4 GHz 10 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg24-rb4186c-efr32xg24-wireless-gecko-radio-board?tab=overview)                              |
| BRD4271A | [EFR32FG25 863-870 MHz 14 dBm Radio Board](https://www.silabs.com/development-tools/wireless/proprietary/fg25-rb4271a-efr32fg25-radio-board?tab=overview)                             |
| BRD4270B | [EFR32FG25 902-928 MHz 16 dBm Radio Board](https://www.silabs.com/development-tools/wireless/proprietary/fg25-rb4270b-efr32fg25-radio-board?tab=overview)                             |
| BRD4194A | [EFR32xG27 2.4 GHz 8 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg27-rb4194a-efr32xg27-8-dbm-wireless-radio-board?tab=overview)                               |
| BRD4401C | [EFR32xG28 868/915 MHz 20 dBm + 2.4 GHz 10 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg28-rb4401c-efr32xg28-2-4-ghz-ble-and-20-dbm-radio-board?tab=overview) |

## Setup ##

1. Generate Project with [Simplicity Studio
   v5](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#example-projects-demos-tab)
   or with [SLC
   CLI](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-tools-slc-cli/).
2. Adjust parameters detailed in the [Configuration](#Configuration) section
   below.
3. Build and flash the project on two boards.

## How It Works ##

The radio operates in receive mode using one of the Direct-to-Buffer modes
specified by the `SL_DIRECT_TO_BUFFER_RX_SOURCE` configuration. In this mode,
the selected specialized data source is continuously captured first into the Rx
FIFO, then moved to a ping-pong buffer allocated by the application. The
transfer of data from the Rx FIFO to the buffer is triggered by the
`RAIL_EVENT_RX_FIFO_ALMOST_FULL` event (this event is also visible on the
`PRS_BUFC_THR1` signal's rising edge) that triggers every time the Rx FIFO is
halfway full.

The size of the Rx FIFO is defined by the `SL_DIRECT_TO_BUFFER_RX_FIFO_SIZE`
macro. The ping-pong buffer size is set to half of the Rx FIFO size, ensuring
that the buffers are alternately filled to capacity. Similarly, the "almost
full" threshold is configured to half the Rx FIFO size, maintaining
synchronization between the Rx FIFO and the buffers.

Since the radio is operating in FIFO Mode (i.e., not in Packet Mode), the
application is unable to receive packets.

### How to use ###

The application can display the most recent segments of the captured data on the
console when the pushbutton on the development kit is pressed. The samples are
formatted in a way that represents the selected specialized data source suitable
for interpretation:

- `RX_IQDATA_FILTMSB` / `RX_IQDATA_FILTLSB`: `"%6d %6d\n"`
- `RX_DEMOD_DATA`: `"%02d"`
- `RX_DIRECT_MODE_DATA` / `RX_DIRECT_SYNCHRONOUS_MODE_DATA`: `"%02x"`

This application does not support changing the Rx Data Source type during
runtime.

### Sampling Frequency ###

In these special data modes the sample rate is dependent on the PHY
configuration and is stored in the generated `radioconf_generation_log.json`
file as either `Raw Data Sampling Rate [sps]` or `Actual Rx Baudrate [baud]`,
depending on the selected data source. Alternatively, it can be measured using
the frequency of the `PRS_BUFC_THR1` PRS signal.

#### Data Transfer Speed Limitations ####

1. Radio -> Rx FIFO: it depends on the HW the radio uses for data capture;
   1. `~1.6 MB/s` (Experimentally, 1/24 HFXO rate), or
   2. \>> `10 MB/s` (~ the magnitude of HFXO rate) - this case is marked by `*`
      in the table below;
2. Rx FIFO -> Application buffer: f_SYSCLK; it's a memcpy instruction;
   1. `~10 MB/s` @SYSCLK=76...80 MHz - DPLL installed (this is enabled by
      default in the application), or
   2. `~5 MB/s` @SYSLCK=38...40 MHz - default SYSCLK configuration
3. App buffer -> host device: depends on the communcation interface (e.g.,
   EFR32xG23 SPI max speed 2*Fpclk = 80 MHz => `10 MB/s`) - printing to the
    console in this application is many times slower than the max speed of the
    interface.

Each cell in the table below indicates the maximum theoretical sample rate at
which samples can be transferred to a host device, assuming a data transfer rate
to the host of 10 MB/s.

| Type                            | Sample rate        | Size of sample | EFR32xG22  [sample/s] | EFR32xG23 [sample/s] | EFR32xG24 [sample/s] | EFR32xG25 [sample/s] | EFR32xG27 [sample/s] | EFR32xG28 [sample/s] |
|---------------------------------|--------------------|----------------|-----------------------|----------------------|----------------------|----------------------|----------------------|----------------------|
| RX_IQDATA_FILTLSB               | f_OSR * 4 Bps      | 4 bytes        | 400 k                 | 400 k                | 400 k                | 400 k                | 400 k                | 400 k                |
| RX_IQDATA_FILTMSB               | f_OSR * 4 Bps      | 4 bytes        | 400 k                 | 2.5 M *              | 2.5 M *              | 2.5 M *              | 400 k                | 2.5 M *              |
| RX_DEMOD_DATA                   | f_OSR * 1 Bps      | 1 byte         | 1.6 M                 | 10 M *               | 10 M *               | Not Supported        | 1.6 M                | 10 M *               |
| RX_DIRECT_MODE_DATA             | f_OSR / 8 Bps      | 1 bit          | Not Supported         | 80 M *               | Not Supported        | 80 M *               | Not Supported        | 80 M *               |
| RX_DIRECT_SYNCHRONOUS_MODE_DATA | f_baudrate / 8 Bps | 1 bit          | Not Supported         | 80 M *               | Not Supported        | 80 M *               | Not Supported        | 80 M *               |

, where f_OSR is the IQ sample rate, and f_baudrate is the baudrate.

You may encounter different issues when the sample rate exceeds the limitations:

- losing samples when limitation 1. is violated: the radio is not able to
  capture all the samples;
- RX FIFO overflow when limitation 2. is violated: Rx FIFO is written faster
  than it's read;

If the Rx FIFO to Application buffer transfer speed is maximized, the CPU
becomes heavily occupied with copying data, potentially preventing the
application from managing other processes effectively.

#### Peripherals Used ####

The application utilizes the default EUSART instance for console logging. The
VCOM enable signal is pre-configured for the tested boards.  

Additionally, it may use up to 3 PRS channels to access the following signals:

- **`PRS_BUFC_THR1` (Rx FIFO almost full)**: Trigger signal for the transfer of
  data from the Rx FIFO to the buffer; this signal is not available on EFR32xG25
platform, the `PRS_RACL_RX` is traced out instead
- **`PRS_MODEM_LRDSALIVE` (async DOUT):** Asynchronously demodulated output
  signal.  
- **`PRS_MODEML_DOUT` (sync DOUT):** Synchronously demodulated output signal.

The `PRS_BUFC_THR1` signal is available at the `EXP_HEADER_15` debug pin, while
the `PRS_MODEM_LRDSALIVE` and `PRS_MODEML_DOUT` signals are traced to the
`EXP_HEADER_11` and `EXP_HEADER_13` debug pins on Wireless Motherboards.

> **Note:** You may need to adjust the PRS and EUSART (VCOM) configurations to
> match the specific board in use.

#### Project Specific Configurations ####

Additional project-specific configurations can be found in the
`<project_root>/config/sl_rail_direct_to_buffer_config.h` file. These
configurations are not currently accessible through the UI as component
settings, so you will need to modify them manually using a text editor or IDE.

#### `SL_DIRECT_TO_BUFFER_RX_FIFO_SIZE` ####

Sets the default Rx FIFO length. Also, sets the almost full and ping-pong
buffers size to half of the Rx FIFO size.

- [The size must be a power of 2 from 64 to
  4096](https://docs.silabs.com/rail/latest/rail-api/efr32-main#receive-and-transmit-fifo-buffers)

#### `SL_DIRECT_TO_BUFFER_RX_SOURCE` ####

Sets the default Rx Source Type.

#### `SL_DIRECT_TO_BUFFER_DEFAULT_CHANNEL` ####

Sets the default channel the communication will take place on.

- Must be within the range defined in the Radio Configuration.

#### Radio Configuration ####

- In order to make `RX_DIRECT_MODE_DATA` or `RX_DIRECT_SYNCHRONOUS_MODE_DATA`
  operational the `RX Direct Mode` must to be set to `SYNC` or `ASYNC`.

## Demo Captures ##

The `doc` folder contains logic analyzer captures of the application running on
various EFR32 Series 2 boards. Each file name specifies the selected data
source, while the traces indicate the corresponding device platform.

These captures illustrate the intended behavior of the system, including the
rate at which samples are collected to the Rx FIFO and transferred to the
ping-pong buffers. Additionally, Direct-to-PRS signals are visible in the traces
on supported platforms.

The radio PHYs (default in SiSDK 2024.06 or enforced on EFR32xG23/24/25/28)
operate with the following sample rates based on device generations:

|                                                                | EFR32xG22 | EFR32xG23 | EFR32xG24 | EFR32xG25 | EFR32xG27 | EFR32xG28 |
|----------------------------------------------------------------|-----------|-----------|-----------|-----------|-----------|-----------|
| **Raw Data Sampling Rate [sps]** (Actual sample frequency)     | 1 M       | 120 k     | 250 k     | 120 k     | 1 M       | 120 k     |
| **Actual Rx Baudrate [baud]** (Desired baudrate)               | 7 M       | 1.625 M   | 1.25 M    | 1.693 M   | 7 M       | 1.625 M   |
| **Actual Demodulator Oversampling Rate** (Target oversampling) | 7         | 13.54     | 5         | 14.105    | 7         | 13.54     |

The frequency of the `PRS_BUFC_THR1` signal reflects the time required to fill
half of the Rx FIFO (512 bytes by default). The width of the high pulses on this
signal approximates the duration needed to transfer data from the Rx FIFO to the
buffer. The following formulas can be used to calculate these rates:

- **`f_transfer_Rx_FIFO`** = 512 / `T` B/s, where `T` is the signal's period.
- **`f_transfer_buffer`** = 512 / `T` B/s, where `T` is the width of the high
  pulse.

When `f_transfer_Rx_FIFO` exceeds the corresponding rate in the table above
(note that the table indicates sample rate, but the actual samples may be 1 bit
to 4 bytes long depending on the data mode), data losses occur, resulting in
missing random samples. This behavior is inherent in the default configuration
of EFR32xG22 and EFR32xG27.

If `f_transfer_buffer` is lower than `f_transfer_Rx_FIFO`, the signal produces
only one high pulse, and generates an overflow event causing the radio to
transition to the idle state automatically (this is not demonstrated in the
attached captures).

In the `*NO_DPLL` capture, the high pulse widths are twice as long than in other
captures, signifying that the transfer rate to the application buffer is halved.

---

By analyzing these metrics, developers can better understand the performance
characteristics and limitations of EFR32 Series 2 boards under various
configurations.

## Notes - Known Issues ##

- For certain development kit and SiSDK version combinations the Radio
  Configuration (i.e., the .radioconf file) might not be added automatically to
  your generated project. In such cases, you should manually copy the file,
  otherwise `RX_DIRECT_MODE_DATA` and `RX_DIRECT_SYNCHRONOUS_MODE_DATA` modes
  might not work on EFR32xG23/25/28 platforms.
- `RX_DEMOD_DATA` on EFR32xG25 devices is currently not operational.
- You may encounter extra 7-800 uA consumption in EM2 mode when using
  `RX_IQDATA_FILTMSB`, `RX_DEMOD_DATA`, `RX_DIRECT_MODE_DATA` or
  `RX_DIRECT_SYNCHRONOUS_MODE_DATA` modes on EFR32xG23/24/25/28 platforms
  (marked by *).
- Currently, there may be data corruption when transitioning from
  `RX_IQDATA_FILTMSB`, `RX_DEMOD_DATA`, `RX_DIRECT_MODE_DATA` or
  `RX_DIRECT_SYNCHRONOUS_MODE_DATA` to `RX_IQDATA_FILTLSB` mode (mode marked by
  \* to normal mode) on EFR32xG23/24/25/28 platforms.
- Currently, Direct-to-PRS functions (i.e., `PRS_MODEML_DOUT` and
  `PRS_MODEM_LRDSALIVE` signals) are not operational when in `RX_IQDATA_FILTLSB`
  mode on EFR32xG23/25/28 platforms. 
- On EFR32xG23/24/25/28 platforms, the IQ sample representation is reversed when
  using `FILTMSB` mode compared to `FILTLSB` (between normal mode and modes
  marked by *). This reversal is accounted for in the application’s console
  output, ensuring the user can interpret the data accurately. Additionally, the
  IQ sample rate may exhibit slight variations between these modes depending on
  the specific PHY configurations.
- An issue related to `Flex - RAIL PRS Support` component has been fixed in
  `SiSDK 2024.06.02` version. If you are using an older version, you may need to
  resolve the building errors. For more information, see the Issue `ID# 1332679`
  in the [Proprietary Flex SDK 3.8.2.0 GA Release
  Notes](https://www.silabs.com/documents/public/release-notes/flex-release-notes-3.8.2.0.pdf).
- Currently, the PHY details in the `radioconf_generation_log.json` log file for
  EFR32xG27 are not populated.
- Configuring Direct-to-Buffer mode (i.e., selecting Rx Data Source other than
  `RX_PACKET_DATA`) in the `RAIL Utility, Initialization` component is not safe,
  because loading the channel may conflict with that configuration. First
  load the channel and call the `RAIL_ConfigData()` afterwards.
