# RAIL Proprietary - Advanced Burst Duty Cycle Receiver #

## Overview ##

This project showcases an Rx-only application using RAIL to receive burst duty
cycle packets without gaps. Each burst packet contains timestamp information for
a scheduled data packet, allowing this receiver to wake up at the right time
upon receiving any burst. This approach improves efficiency by enabling precise
synchronization while minimizing power consumption.

To get more information about the topic of low duty cycle, read our [related
article](https://docs.silabs.com/rail/latest/rail-training-low-duty-cycle/).

> [!IMPORTANT]  
> This application lacks error handling and other critical features for
> readability. It is intended solely for educational purposes and is not
> recommended as a foundation for development.

## SDK version ##

SiSDK 2025.6.0 and above

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
| BRD4194A | [EFR32xG27 Wireless 2.4 GHz +8 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg27-rb4194a-efr32xg27-8-dbm-wireless-radio-board?tab=overview) |
| BRD4401C | [EFR32xG28 868/915 MHz 20 dBm + 2.4 GHz 10 dBm Radio Board](https://www.silabs.com/development-tools/wireless/xg28-rb4401c-efr32xg28-2-4-ghz-ble-and-20-dbm-radio-board?tab=overview) |

## Setup ##

1. Generate Project with [Simplicity Studio
   v5](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#example-projects-demos-tab)
   or with [SLC
   CLI](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-tools-slc-cli/).
2. Adjust parameters detailed in the [Configuration](#configuration) section
   below.
3. Build and flash the project on a board.

## How It Works ##

This project demonstrates how to receive burst duty cycle packets with
timestamping information of a data packet with RAIL. The example uses two packet
types: the burst packet - which is repeated over and over with a minimal change
in the payload that contains the timestamp information when the data packet is
expected, and the data packet itself. To use this application you need two
nodes, one with the transmitter and one with the receiver program.

This program provides information via the terminal, including received packet
payload and received wake-up packet event.

Note, that this is not a DMP (Dynamic Multiprotocol) project! The functional
part of the code is separated to make sure that it can be used in DMP projects
if needed.

> `SL_ADVANCED_BURST_DC_RECEIVER_MIN_DATA_LEN` is a variable used to define the
> minimum payload length for reception in the anchored data frame.
> `SL_ADVANCED_BURST_DC_RECEIVER_MAX_PACKET_LEN` is the maximum packet length to
> process. This value must be smaller than
> `SL_ADVANCED_BURST_DC_RECEIVER_BUFFER_LENGTH`. The length of the Tx buffer
> should be set according to the radio configuration using this parameter. The
> `SL_ADVANCED_BURST_DC_RECEIVER_DEFAULT_CHANNEL` parameter determines the index
> of the channel that is used for packet reception (see available channels in
> the radio_settings.radioconf file of the project). The scheduling parameters
> can be configured with `SL_ADVANCED_BURST_DC_RECEIVER_WAKEUP_PRIORITY` (Rx
> window priority), `SL_ADVANCED_BURST_DC_RECEIVER_WAKEUP_SLIPTIME_US` and
> `SL_ADVANCED_BURST_DC_RECEIVER_RX_PRIORITY` (Scheduled Rx priority). The
> timing of the reception of the data message can be further adjusted by
> `SL_ADVANCED_BURST_DC_RECEIVER_WAKE_EARLY_ON_NO_TIMESTAMP_US` and
> `SL_ADVANCED_BURST_DC_RECEIVER_WAKE_EARLY_FOR_DATA_MSG_US`. The burst on time
> and burst cycle time of the duty cycling can be configured with
> `SL_ADVANCED_BURST_DC_RECEIVER_BURST_ON_TIME_US` and
> `SL_ADVANCED_BURST_DC_RECEIVER_BURST_CYCLE_TIME_US`.

### How to use ###

To enable or disable sleeping, you need to press the BTN0 button. You can see
the difference between the two modes in power consumption in Energy Profiler.

### Configuration ###

#### Peripherals Used ####

The application utilizes the default EUSART instance for CLI communication and
console logging. The VCOM enable signal is pre-configured for the tested boards.

You may want to adjust the EUSART (VCOM) configurations according to the
board you are using.

#### Project Specific Configurations ####

Additional project-specific configurations can be found in the
`<project_root>/config/sl_rail_advanced_burst_duty_cycle_receiver_config.h`
file. These configurations are not currently accessible through the UI as
component settings, so you will need to modify them manually using a text editor
or IDE.

#### `SL_ADVANCED_BURST_DC_RECEIVER_MIN_DATA_LEN` ####

Minimum acceptable frame length with data frame.

#### `SL_ADVANCED_BURST_DC_RECEIVER_BUFFER_LENGTH` ####

Sets Rx Buffer length in bytes. This should be enough to hold the data packet,
so at least SL_ADVANCED_BURST_DC_RECEIVER_MIN_DATA_LEN.

- [The size must be a power of 2 from 64 to
  4096](https://docs.silabs.com/rail/latest/rail-api/efr32-main#receive-and-transmit-fifo-buffers)

#### `SL_ADVANCED_BURST_DC_RECEIVER_DEFAULT_CHANNEL` ####

Set the index of the default channel used to receive.

- Only configured channel index can be used (see available channels in the
  radio_settings.radioconf file of the project).

#### `SL_ADVANCED_BURST_DC_RECEIVER_WAKEUP_PRIORITY` ####

Priority of the Rx windows. If other higher priority scheduled Rx or Tx is
happening at the same time, the window will be delayed, which may cause missing
packets.

#### `SL_ADVANCED_BURST_DC_RECEIVER_WAKEUP_SLIPTIME_US` ####

Sliptime of the Rx window. It could theoretically add PER (Packet Error Rate),
but since the probability is low, it is recommended to allow the other protocol
to delay the window. In microseconds.

#### `SL_ADVANCED_BURST_DC_RECEIVER_RX_PRIORITY` ####

Priority of the the scheduled Rx for burst. Prio 0 is most important.

#### `SL_ADVANCED_BURST_DC_RECEIVER_MAX_PACKET_LEN` ####

Maximum packet length to process. Above it will generate
RAIL_SLEEPDC_STATUS_PACKET_TOO_LONG.

#### `SL_ADVANCED_BURST_DC_RECEIVER_WAKE_EARLY_ON_NO_TIMESTAMP_US` ####

If timestamp cannot be recovered, we use the current timestamp when processing
the packet for anchor, which will be delayed from the actual timestamp. This
will be compensated by the below value for scheduled Rx.

#### `SL_ADVANCED_BURST_DC_RECEIVER_WAKE_EARLY_FOR_DATA_MSG_US` ####

In burst mode, the data message is tightly scheduled to be transmitted at anchor
time. However we need to start Rx a bit earlier to make sure state transition
time is done when the preamble starts. This doesn't need to be much though. In
microseconds.

#### `SL_ADVANCED_BURST_DC_RECEIVER_BURST_CYCLE_TIME_US` ####

Period time in microseconds in burst mode. Equals transmission time or on time plus off
time.

#### `SL_ADVANCED_BURST_DC_RECEIVER_BURST_ON_TIME_US` ####

On time in burst mode. Should be burst packet length plus some margin.
