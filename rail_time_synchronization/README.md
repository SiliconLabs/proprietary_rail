# RAIL Proprietary - Time Synchronization #

## Overview ##

This project demonstrates remote time synchronization in RAIL's timebase.

To get more information about the topic of remote time synchronization, read our
[related
article](https://community.silabs.com/s/article/time-synchronization-with-rail-example-project-demonstration?language=en_US).

## SDK version ##

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
| BRD4182A | [EFR32xG22 2.4 GHz 6 dBm Radio Board](https://www.silabs.com/development-tools/wireless/slwrb4182a-efr32xg22-wireless-gecko-radio-board?tab=overview) |
| BRD4204D | [EFR32ZG23 868-915 MHz 14 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg23-rb4204d-efr32xg23-868-915-mhz-14-dbm-radio-board?tab=overview)|
| BRD4186C | [EFR32xG24 2.4 GHz 10 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg24-rb4186c-efr32xg24-wireless-gecko-radio-board?tab=overview)|
| BRD4271A | [EFR32FG25 863-870 MHz 14 dBm Radio Board](https://www.silabs.com/development-tools/wireless/proprietary/fg25-rb4271a-efr32fg25-radio-board?tab=overview) |
| BRD4194A | [EFR32xG27 2.4 GHz 8 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg27-rb4194a-efr32xg27-8-dbm-wireless-radio-board?tab=overview) |
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

> This example builds on the [RAIL Tutorial: Time, Timestamping and
> Scheduling](https://docs.silabs.com/rail/latest/rail-training-timestamping/)
> and [RAIL Tutorial: Downloading Received
> Messages](https://docs.silabs.com/rail/latest/rail-training-trx/) tutorials.
> Please read them first if you haven't already.

----

This project demonstrates how a distant node can share its local, `System Time`
with another node(s) and achieve time synchronization with RAIL. To use this
application you need at least two nodes: a transmitter and a receiver node,
although the roles are interchangeable.

Each node maintains its System Time with the use of the microsecond-based [RAIL
Timer](https://docs.silabs.com/rail/latest/rail-api/system-timing). The devices
generate a periodic tick signal with a 1-second interval, aligned precisely to
the RAIL Timer's 1-second boundaries, establishing a fixed point in time.

During transmission, the transmitter embeds a precise timestamp in the packet,
representing its System Time at the moment the last sync word bit is
transmitted.

> The position of the timestamp in the packet is determined by the
> `SL_TIME_SYNCHRONIZATION_PACKET_OFFSET` configuration parameter. Also, the
> preamble and sync word length should be set according to the radio
> configuration in the `SL_TIME_SYNCHRONIZATION_PREABMLE_LENGTH_TOTAL` and
> `SL_TIME_SYNCHRONIZATION_SYNCWORD_LENGTH_TOTAL` parameters.

The receiver also records the timestamp of the last sync word bit as captured in
its own System Time. Using these timestamps, the receiver determines the time
difference between the transmitter’s and its own System Time.

> The demodulator propagation delay is accounted for in the
> `SL_TIME_SYNCHRONIZATION_PROPAGATION_DELAY_US` configuration parameter.

After calculating the time difference, the receiver adjusts its tick signal to
synchronize perfectly with the transmitter’s tick signal, demonstrating
successful time synchronization.

----

Since RAIL operates on a High-Frequency (HF) clock source, the synchronization
accuracy depends on the HF crystal oscillator (XO) frequency tolerance.
Typically, the XO frequency tolerance is in the range of 10–20 ppm, meaning the
frequency can deviate by 10–20 Hz from the nominal clock frequency of 38.4–40
MHz. Over time, this can result in synchronization errors accumulating at a rate
of 10–20 microseconds per second.

### How to use ###

To initiate sharing the System Time in the packet, you need to press the PB0
button on or issue the `start` CLI command the transmitter node. When the packet
arrives the receiver calculates the time difference and start generating the
tick-signal synchronized with the transmitter's tick-signal.

By design the nodes are always listening for packets, except when they are
transmitting the synchronization packet.

Along with the tick-signal generation, the devices log their System Time to the
console in the same interval. Once synchronized, the remote node's System Time
is also displayed on the console. Additionally, the application features the
`getTime` CLI command to read the current System Time of the node.

### Configuration ###

#### Peripherals Used ####

The Application uses the default EUSART instance for CLI communication and
console logging. The VCOM enable signal is also configured for the tested
boards.

The application utilizes the `PRS_PROTIMERL_CC2` signal to generate the tick
signal and the OR-combined `PRS_MODEMH_SYNCSENT` and `PRS_MODEML_FRAMEDET`
signals to mark the start of payload transmission and packet detection,
respectively. These PRS signals are traced via GPIO to the EXP Header 15 and 16
debug pins.

You may want to adjust the PRS and EUSART (VCOM) configurations according to the
board you are using.

#### Project Specific Configurations ####

Additional project-specific configurations can be found in the
`<project_root>/config/sl_rail_time_synchronization_config.h` file. These
configurations are not currently accessible through the UI as component
settings, so you will need to modify them manually using a text editor or IDE.

#### `SL_TIME_SYNCHRONIZATION_TX_FIFO_SIZE` ####

Sets the default Tx FIFO length.

- [The size must be a power of 2 from 64 to
  4096](https://docs.silabs.com/rail/latest/rail-api/efr32-main#receive-and-transmit-fifo-buffers)

#### `SL_TIME_SYNCHRONIZATION_PACKET_LENGTH` ####

Sets the default packet length.

- Set it according to the Radio Configuration. Fixed-length config only.
- Must not exceed `SL_TIME_SYNCHRONIZATION_TX_FIFO_SIZE`.

#### `SL_TIME_SYNCHRONIZATION_PACKET_OFFSET` ####

Position of the timestamp in the packet. The timestamp is a 32-bit value.

- Must be equal or less than `SL_TIME_SYNCHRONIZATION_PACKET_LENGTH - 4`.
- A higher bitrate requires a larger offset, as calculating and embedding the
  timestamp into the packet must be completed before that portion of the packet
  is transmitted.

> NOTE: Currently, the only supported value is
`SL_TIME_SYNCHRONIZATION_PACKET_OFFSET = SL_TIME_SYNCHRONIZATION_PACKET_LENGTH -
4`

#### `SL_TIME_SYNCHRONIZATION_PREABMLE_LENGTH_TOTAL` ####

Total length of the preamble in bits.

- Set it according to the Radio Configuration.

#### `SL_TIME_SYNCHRONIZATION_SYNCWORD_LENGTH_TOTAL` ####

Total length of the sync word in bits.

- Set it according to the Radio Configuration.

#### `SL_TIME_SYNCHRONIZATION_PROPAGATION_DELAY_US` ####

Demodulator propagation delay in microseconds.

- The propagation delay can be measured on the `RADIOPRS_SYNCSENT_PRS_CHANNEL`
  and `RADIOPRS_FRAMEDET_PRS_CHANNEL` channels.
- The `<project_root>/autogen/radioconf_generation_log.json` file may include an
  estimation for the demodulator propagation delay, which can be converted to
  microseconds using `(Signal Propagation Delay [symbols]` and `Actual Rx
  Baudrate [baud]` variables.

#### `SL_TIME_SYNCHRONIZATION_DEFAULT_CHANNEL` ####

Sets the default channel the communication will take place on.

- Must be within the range defined in the Radio Configuration.

## Demo Captures ##

In the `doc` folder you can find 3 logic analyzer captures of the application
running on two EFR32 Series 2 boards. Each of these were captured at a subGHz
PHY using 500 kbps bitrate and 22 us propagation delay.

You can open these files with [Saleae - Logic
2](https://www.saleae.com/pages/downloads) software.

----

The `time_sync_start_capture.sal` file illustrates the beginning of the
synchronization process. Initially, Node #1 boots up and starts generating its
tick signal. A few seconds later, Node #2 boots up and begins generating its own
tick signal.

At marker P0, Node #1 transmits a synchronization packet, which Node #2
receives. The propagation delay is captured at marker P2. After receiving the
packet, Node #2 synchronizes its tick signal with Node #1’s tick signal. The
initial synchronization error, marked at P3, is approximately 16 µs.

Subsequently, at another P2 marker, Node #2 shares its System Time with Node #1,
enabling Node #1 to synchronize its tick signal to Node #2’s tick signal. The
complete console log for this process is included in the doc folder.

----

The `time_sync_long_capture.sal` file demonstrates how synchronization error
accumulates over time. It extends the previous capture, running the nodes for
over 2500 seconds. At marker P0, the synchronization error is approximately 560
µs. After several minutes, the error increases to 1634 µs, indicating an XO
frequency difference of 0.43 ppm, calculated as (1634-560) / 2,500,000,000.

----

The `time_sync_3_nodes_capture.sal` file illustrates synchronization with three
nodes. Each node maintains the last received System Time from the others. At
marker P3, Node #1 sends a synchronization packet, prompting all three nodes to
align their tick signals with Node #1’s signal.

After Node #2 transmits a packet (P4), Node #3 updates its synchronization
source to Node #2, and Node #1 also aligns its tick signal with Node #2.
Similarly, when Node #3 transmits a packet (P5), both Node #1 and Node #2 switch
their synchronization source to Node #3.

## Notes ##

- An issue related to `Flex - RAIL PRS Support` component has been fixed in
  `SiSDK 2024.06.02` version. If you are using an older version, you may need to
  resolve the building errors. For more information, see the Issue `ID# 1332679`
  in the [Proprietary Flex SDK 3.8.2.0 GA Release
  Notes](https://www.silabs.com/documents/public/release-notes/flex-release-notes-3.8.2.0.pdf).
