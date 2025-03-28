# RAIL Proprietary - Direct Mode Packet Detector #

## Overview ##

This example application showcases how the radio detects and decodes standard
packets by monitoring the stream of asynchronously demodulated signals.

For more details on Direct Mode, visit the [RAIL Developer's Guide
page](https://docs.silabs.com/rail/latest/rail-developers-guide-direct-mode/).

## SDK Version ##

SiSDK 2024.6.2 and above

## Hardware Required ##

EFR32 Series 2

## Connections Required ##

Connect at least two identical Development Kits to your PC. Additionally, you
may want to connect a logic analyzer to the debug pins and/or open a serial
terminal connected to the VCOM port.

### Tested boards for working with this example ###

| Board ID | Description  |
| ---------------------- | ------ |
| BRD4204D | [EFR32ZG23 868-915 MHz 14 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg23-rb4204d-efr32xg23-868-915-mhz-14-dbm-radio-board?tab=overview)|
| BRD4271A | [EFR32FG25 863-870 MHz 14 dBm Radio Board](https://www.silabs.com/development-tools/wireless/proprietary/fg25-rb4271a-efr32fg25-radio-board?tab=overview) |
| BRD4270B | [EFR32FG25 902-928 MHz 16 dBm Radio Board](https://www.silabs.com/development-tools/wireless/proprietary/fg25-rb4270b-efr32fg25-radio-board?tab=overview) |
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

At initialization, the radio is configured to continuously output the
asynchronously demodulated signal stream (async DOUT - `PRS_MODEM_LRDSALIVE`) on
a designated PRS (Peripheral Reflex System) channel. The high and low pulses of
this signal are measured using two TIMER instances.

Given a known baud rate, the pulse lengths are translated into symbols, which
are sequentially stored in a buffer. When the buffer contains a pattern matching
a configured detection mask (referred to here as the "syncword," though this
usage may differ slightly from its conventional meaning in the radio context), a
packet is considered detected.

Both the detection mask and syncword pattern are configurable, allowing for
flexibility in various application scenarios.

This application operates in two distinct modes: **Detection Only** and **Packet
Reception**.

1. Detection Only Mode: In this mode, the radio detects the presence of packets
but does not decode their contents. It showcases the radio's capability to
identify packets by measuring the pulse lengths of the asynchronously
demodulated signal.
2. Packet Reception Mode Here, the radio goes a step further. After detecting a
packet, it decodes the subsequent data and outputs the received packet to the
console. Due to the additional processing required, this mode operates slightly
slower than Detection Only mode.

For comparison, the application also outputs the synchronous demodulated signal
(`PRS_MODEML_DOUT`). This allows you to compare asynchronous detection with
packet mode. Essentially, if the packet appears on this trace, the hardware
packet detector would be able to detect it.

This application requires radio configurations that enable Rx Direct Mode.

----

The application also supports transmitting packets. Pressing the PB0 button on
the transmitter board sends a fixed `SL_DIRECT_MODE_DETECTOR_PACKET_LENGTH` long
packet with a configurable payload pattern
(`SL_DIRECT_MODE_DETECTOR_PAYLOAD_PATTERN`) on the designated channel
(`SL_DIRECT_MODE_DETECTOR_DEFAULT_CHANNEL`).

----

The application also features debug GPIOs that indicate when packets are
detected and measure the time spent in the TIMERs' interrupt handlers. You can
enable it through the `SL_DIRECT_MODE_DEBUG_GPIO` configuration entry.

### How to use ###

1. Select the Operational Mode

Set the desired mode of operation by configuring
`SL_DIRECT_MODE_DETECTOR_APP_MODE`.

2. Configure the Syncword and Mask

Define the syncword pattern and mask in the configuration file.

3. Choose radio PHY

- Protocol 0 uses an OOK configuration.
- Protocol 1 uses an FSK PHY configuration.

These protocols utilize the default syncword settings initially configured for
the application. Select the protocol via the
`SL_RAIL_UTIL_INIT_PROTOCOL_PROPRIETARY_INST0_INDEX` configuration.

4. Transmit a Packet

Press the PB0 button on the transmitter board to send a packet.

----

On the receiver node you may expect the following messages on the console.

In Detection Only mode:

```
[I] Packet Detected.
```

In Packet Reception mode:

```
[I] Packet received:
96 96 96 96 96 96 96 96 96 96 96 96 96 96 96 96 
```

> 0x96 is the default payload pattern configured with
> `SL_DIRECT_MODE_DETECTOR_PAYLOAD_PATTERN`.

----

You should see the following message on the Tx node console whenever the button
is pressed.

```
[I] Packet transmission initiated.
[I] Packet transmission completed.
```

#### Further Use Cases for this Example ####

This example can be used to:

1. Compare the capabilities and performance of packet mode versus direct mode.
2. Experiment with determining the shortest preamble length the radio can
   detect, which may be undetectable in packet mode.
3. Develop custom PWM modulation decoding algorithms using this example as a
   starting point.
4. Develop learning algorithms for custom modulation schemes.

### Configuration ###

#### Peripherals Used ####

The application utilizes the default EUSART instance for CLI communication and
console logging. The VCOM enable signal is pre-configured for the tested boards.  

Additionally, it uses two PRS channels to access the following signals:

- **`PRS_MODEM_LRDSALIVE` (async DOUT):** Asynchronously demodulated output
  signal.  
- **`PRS_MODEML_DOUT` (sync DOUT):** Synchronously demodulated output signal.

Two GPIO pins are also configured to trace these PRS signals to the standard
`EXP_HEADER_15` and `EXP_HEADER_16` debug pins on Wireless Motherboards.

The application employs the **TIMER0** and **TIMER1** instances to measure the
high and low pulse durations of the async DOUT signal.

> **Note:** You may need to adjust the PRS and EUSART (VCOM) configurations to
> match the specific board in use. However, the timer instances are fixed and
> cannot be modified.

#### Project Specific Configurations ####

Additional project-specific configurations can be found in the
`<project_root>/config/sl_rail_direct_mode_detector_config.h` file. These
configurations are not currently accessible through the UI as component
settings, so you will need to modify them manually using a text editor or IDE.

#### `SL_DIRECT_MODE_DETECTOR_TX_FIFO_SIZE` ####

Sets the default Tx FIFO length.

- [The size must be a power of 2 from 64 to
  4096](https://docs.silabs.com/rail/latest/rail-api/efr32-main#receive-and-transmit-fifo-buffers)

#### `SL_DIRECT_MODE_DETECTOR_PACKET_LENGTH` ####

Sets the default packet length in bytes for both Tx and RX.

- Set it according to the Radio Configuration. Fixed-length config only.

#### `SL_DIRECT_MODE_DETECTOR_PAYLOAD_PATTERN` ####

Sets the payload pattern. 

- For `OOK` modulation, the payload pattern should have several `1` symbols to
ensure the receiver can detect the packet.

#### `SL_DIRECT_MODE_DEBUG_GPIO` ####

Enables the debug GPIOs.

#### `SL_DIRECT_MODE_DETECTOR_SYNCWORD_PATTERN` ####

Sets the 32-bit syncword pattern to detect the packet.

- False detection rate grows as the syncword gets shorter. The syncword should
  be long enough to avoid false detections.

#### `SL_DIRECT_MODE_DETECTOR_SYNCWORD_MASK` ####

Sets the 32-bit syncword mask.

- Match the syncword pattern.
- If too few bits are masked out, false syncword detections may occur more
  frequently.

#### `SL_DIRECT_MODE_DETECTOR_APP_MODE` ####

Sets the application mode.

- `SL_DETECT_ONLY`: Detection only
- `SL_PACKET_RECEIVE`: Packet receive

#### `SL_DIRECT_MODE_DETECTOR_DEFAULT_CHANNEL` ####

Sets the default channel the communication will take place on.

- Must be within the range defined in the Radio Configuration.

#### Radio Configuration ####

- `RX Direct Mode` must to be set to `SYNC`.
- The practical limitation on baud rate is 500 kbps for Packet Reception mode,
  and slightly higher for Detection Only mode. It depends on how much time the
  core spends in the algorithm which is also directly related to the SYSCLK
  rate.

## Demo Captures ##

The Logic analyzer captures that can be found within the doc folder show packet
exchanges between an EFR32xG23 (device #1) and an EFR32xG25 (device #2) board.
For these capture all debug utilities were enabled.

----

The OOK_high/mid/low_power.sal captures illustrate why detecting higher-power
signals takes more time. To handle stronger signals, the radio performs
additional AGC (Automatic Gain Control) iterations to adjust the gain and
stabilize the async DOUT signal.

At very high power levels, this process can take up to 10–12 bits at the default
baud rate. In contrast, at lower power levels where AGC is inactive, the radio
can correctly demodulate the first high bit without delay.

----

A similar delay can be observed when using the FSK (Protocol 1) configurations.
The FSK_freq_offset_50/25/0KHz.sal captures were taken with the same
transmission power but different frequency offsets, set to 50 kHz, 25 kHz, and 0
kHz, respectively.

The captures demonstrate that the greater the frequency offset, the longer it
takes for the EFR32xG25 receiver to stabilize the demodulated signal.

----

You can also observe that the pulses on the EFR32xG25's algorithm active debug
signal are approximately twice as narrow (~1.1 µs) compared to those on the
EFR32xG23 (~2.4 µs) during preamble signal reception. This difference arises
because the SYSCLK on the EFR32xG23 runs at the HFXO rate by default, whereas in
this example, the EFR32xG25 operates on a different clock source, close to 100
MHz. Again, this correlates to the maximum baud rate achievable by the
algorithm.

> Note that while transmitting only sync DOUT is active.

## Notes ##

- For extremely low baud rates, the 16-bit timers may overflow during pulse
  measurement if the TIMERs operate at their default highest frequency. To
  address this, you may need to apply prescaling to the TIMERs and adjust the
  `symbol_duration` calculation accordingly.
- An issue related to `Flex - RAIL PRS Support` component has been fixed in
  `SiSDK 2024.06.02` version. If you are using an older version, you may need to
  resolve the building errors. For more information, see the Issue `ID# 1332679`
  in the [Proprietary Flex SDK 3.8.2.0 GA Release
  Notes](https://www.silabs.com/documents/public/release-notes/flex-release-notes-3.8.2.0.pdf).
