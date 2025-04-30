# RAIL Proprietary - RF Generator #

## Overview ##

This project shows the implementation of a simple CLI (Command Line
Interface)-controlled  RF Generator, which is able to send user-defined packet
sequences.

Note, that there is no runtime protocol index change (Protocol-based Multi-PHY)
support in this application.

## SDK version ##

SiSDK 2024.6.0 and above

## Hardware Required ##

EFR32 Series 2

## Connections Required ##

Connect at least two identical Development Kits to your PC and open two serial
terminal connected to the VCOM ports.

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
2. Adjust parameters detailed in the [Configuration](#configuration) section
   below.
3. Build and flash the project on one of the boards (Tx).
4. Create a "RAIL - SoC Simple TRX" project for the other radio board (Rx). Use
   the default project settings.

## How It Works ##

A sequence is defined by packets and the delays between the packets. The packets
are identified by their ID number and they can be arbitrarily combined to build
up a sequence. The sequence can be sent repeatedly a specified number of times
or infinitely. For example:

```
Tx packet ID1->300ms delay->Tx PACKET ID3->100ms delay->Tx packet ID2->600ms...
Tx packet ID4->100ms delay->Tx packet ID1->50ms...
```

Each packet has its content, length and channel. All of them are configurable at
runtime offering a very versatile and flexible application for testing. The
packets and the sequence can be modified with the predefined CLI commands.

This example always uses fixed-length packet configuration as it is enforced by
RAIL in the schedule_next_tx function.

> `SL_RF_GENERATOR_PAYLOAD_LENGTH` is a variable used to define the initial
> payload length for transmission in the application. Its value should align
> with the payload length determined by the radio configuration when using a
> fixed-length configuration. Additionally, it must not exceed the value of
> `SL_RF_GENERATOR_MAX_PAYLOAD_LENGTH` or `SL_RF_GENERATOR_FIFO_LENGTH`. The
> length of the Tx buffer should be set according to the radio configuration
> using this parameter. The `SL_RF_GENERATOR_DEFAULT_CHANNEL` parameter
> determines the index of the channel that is used for packet transmission (see
> available channels in the radio_settings.radioconf file of the project).
> `SL_RF_GENERATOR_PACKET_DELAY_US` determines the initial delay between packet
> transmissions, `SL_RF_GENERATOR_NUMBER_OF_PACKETS` sets the number of packets
> at startup (this is the amunt that is stored in the memory) and
> `SL_RF_GENERATOR_SEQUENCE_LENGTH` sets the index of the default channel used
> to transmit.

### How to use ###

#### CLI commands ####

- help: List the available CLI commands.
- setPayload: Set the packet's payload bytes for future transmits.\
*[uint8] packetID*\
*[uint16] offset*\
*[uint8opt] byte0 byte1 ...*
- getPayload: Print the packet's payload data and byte length.\
*[uint8] packetID*
- setLength: Set the packet's length.\
*[uint8] packetID*
- setChannel: Set packet's Tx channel.\
*[uint8] packetID*
- getPacketInfo: Print packet's info.\
*[uint8] packetID*
- setSeq: Set the Tx sequence.\
*[uint32opt] packetID0 delayUs0 payketID1 delayUs1*
- getSeq: Print the current Tx sequence.
- setRepeat: Set sequence repeat time. 0 means infinite repeat time.\
*[uint32] repeat*
- getRepeat: Print sequence repeat time.
- tx: Start/Stop sequence Tx.
- txLog: Enable Tx printing messages.\
*[uint8opt] enable*
- enterscript: Enter script entry mode. Conclude entry mode with text 'endScript'..\
*[uint8opt] [0=RAM] 1=Flash-script will run on boot*
- printScript: Print the script entered via enterScript.\
*[uint8opt] [0=RAM] 1=Flash*
- clearscript: Clear the script entered via enterScript.\
*[uint8opt] [0=RAM] 1=Flash*
- runScript: Run the script entered via enterScript.\
*[uint8opt] [0=RAM] 1=Flash-script will run on boot*
- wait: Suspend processing of CLI input for a while.\
*[uint32] waitTimeUs*\
*[stringopt] ['rel'=Relative] 'abs'=Absolute*

#### Command Scripting ####

The RF Generator also supports command scripting the same way as [RAILtest
does](https://www.silabs.com/documents/public/user-guides/ug409-railtest-users-guide.pdf#page=6).

#### Testing ####

On startup the **RF Generator (Tx) node** will be displayed:

```
> RF Generator
```

The **Receiver (Rx) node** will be displayed:

```
> Simple TRX
```

The devices start in receive state.

On the **RF Generator (Tx) node** issue the `setPayload 1 0 0xAA 0xBB` command
on the Tx node to modify the payload of the ID 1 packet starting from offset 0:

```
setPayload 1 0 0xAA 0xBB
> {{(setpayload)}{len: 16}{payload: 0xaa 0xbb 0x11 0x22 0x33 0x44 0x55 0x0f 0x77 0x88 0x99 0xaa 0xbb 0xcc 0xdd 0xee}}
```

On the **RF Generator (Tx) node** issue the `setLength 1 10` command
on the Tx node to modify the payload length of the ID 1 packet:

```
setLength 1 10
> {{(setlength)}{packet id: 1}{ len: 10}}
```

On the **RF Generator (Tx) node** issue the `getPacketInfo 1` command
on the Tx node to get every information of the ID 1 packet:

```
getPacketInfo 1
> {{(getpacketinfo)}{packet id: 1}{ channel: 0}{ len: 10}{payload: 0xaa 0xbb 0x11 0x22 0x33 0x44 0x55 0x0f 0x77 0x88}}
```

On the **RF Generator (Tx) node** issue the `setSeq 1 1000000 2 2000000` command
on the Tx node to set a sequence of 2 packets:

```
setSeq 1 1000000 2 2000000
> {{(setseq)}{sequence: id: 1 delay: 1000000 Us id: 2 delay: 2000000 Us}}
```

On the **RF Generator (Tx) node** issue the `setRepeat 2` command on the Tx node
to set the repeat count of the sequence:

```
setRepeat 2
> {{(setrepeat)}{Repeat number: 2}}
```

On the **RF Generator (Tx) node** issue the `tx` command on the Tx node to
request/stop packet transmission:

```
tx
> [I] Packet 1 Tx has been started
[I] Packet 2 Tx has been started
[I] Packet 1 Tx has been started
[I] Packet 2 Tx has been started
```

On the receiver side the following response will be printed out on packet
reception:

```
> Packet has been received: 0x0F, 0x0E, 0x11, 0x22, 0x33, 0x44, 0x55, 0x0F, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE
Packet has been received: 0x0F, 0x0E, 0x11, 0x22, 0x33, 0x44, 0x55, 0x0F, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE
```

Note that the only the ID 2 packet will be received in the example, as the
packet length is modified in packet ID 1.

**Note:** You can press the Button 0 to request/stop packet transmission
respectively. Led 1 will toggle on every packet transmission.

### Configuration ###

#### Peripherals Used ####

The application utilizes the default EUSART instance for CLI communication and
console logging. The VCOM enable signal is pre-configured for the tested boards.

You may want to adjust the EUSART (VCOM) configurations according to the
board you are using.

#### Project Specific Configurations ####

Additional project-specific configurations can be found in the
`<project_root>/config/sl_rail_rf_generator_config.h` file. These configurations
are not currently accessible through the UI as component settings, so you will
need to modify them manually using a text editor or IDE.

#### `SL_RF_GENERATOR_PAYLOAD_LENGTH` ####

Sets initial payload length in bytes. This is limited by
SL_RF_GENERATOR_MAX_PAYLOAD_LENGTH.

#### `SL_RF_GENERATOR_MAX_PAYLOAD_LENGTH` ####

Sets the maximum payload length in bytes. This is limited by
SL_RF_GENERATOR_FIFO_LENGTH.

#### `SL_RF_GENERATOR_FIFO_LENGTH` ####

Sets Tx FIFO length in bytes.

- [The size must be a power of 2 from 64 to
  4096](https://docs.silabs.com/rail/latest/rail-api/efr32-main#receive-and-transmit-fifo-buffers)

#### `SL_RF_GENERATOR_DEFAULT_CHANNEL` ####

Set the index of the default channel used to transmit and receive.

- Only configured channel index can be used (see available channels in the
  radio_settings.radioconf file of the project).
  
#### `SL_RF_GENERATOR_PACKET_DELAY_US` ####

Sets the initial delay between the packets.

#### `SL_RF_GENERATOR_NUMBER_OF_PACKETS` ####

Set the initial number of packets. Only this amunt will be stored in memory.

#### `SL_RF_GENERATOR_SEQUENCE_LENGTH` ####

Sets the number of packets present in a sequence. It must be smaller or equal to
SL_RF_GENERATOR_NUMBER_OF_PACKETS.
