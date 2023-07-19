# RF Generator #

![Type badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_rf_generator_common.json&label=Type&query=type&color=green)
![Technology badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_rf_generator_common.json&label=Technology&query=technology&color=green)
![License badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_rf_generator_common.json&label=License&query=license&color=green)
![SDK badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_rf_generator_common.json&label=SDK&query=sdk&color=green)
![Build badge](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_rf_generator_build_status.json)
![Flash badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_rf_generator_common.json&label=Flash&query=flash&color=blue)
![RAM badge](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/SiliconLabs/application_examples_ci/master/proprietary_rail/rail_rf_generator_common.json&label=RAM&query=ram&color=blue)
## Summary ##

This project shows the implementation of a simple CLI (Command Line Interface)-controlled  RF Generator, which is able to send user-defined packet sequences.

## Gecko SDK version ##

Gecko SDK Suite v4.2.1

## Hardware Required ##

- 2 x [Wireless Starter Kit Mainboard](https://www.silabs.com/development-tools/wireless/wireless-starter-kit-mainboard)

- 2 x [10dBm EFR32MG12 Radio Board](https://www.silabs.com/development-tools/wireless/zigbee/slwrb4162a-efr32mg12-radio-board). One is the RF Generator (TX) node, one is the Receiver (RX) node.

## Connections Required ##

- Connect the radio board to the WSTK mainboard.

- Connect the WSTK mainboard to the PC through a USB cable.

## Setup ##

To test this application, you can either import the provided project files:

- [RF Generator (TX) node](SimplicityStudio/rail_rf_generator.sls)
- [Receiver (RX) node](SimplicityStudio/rail_soc_simple_trx.sls)

If you want to create the projects from scratch for a different radio board or kit, follow these steps:

- **Receiver (RX) node**
  - Create a "RAIL - SoC Simple TRX" project for the EFR32MG12 Radio Board(BRD4162A) using Simplicity Studio v5. Use the default project settings. Be sure to connect and select the BRD4162A Board from the "Debug Adapters" on the left before creating a project

- **RF Generator (TX) node**
  - Please follow this section: [How to Port to Another Part](#how-to-port-to-another-part)

Compile the projects and download the application to two radio boards.

## How It Works ##

A sequence is defined by packets and the delays between the packets. The packets are identified by their ID number and they can be arbitrarily combined to build up a sequence. The sequence can be sent repeatedly a specified number of times or infinitely.
For example:

```
TX packet ID1->300ms delay->TX PACKET ID3->100ms delay->TX packet ID2->600ms...
TX packet ID4->100ms delay->TX packet ID1->50ms...
```

Each packet has its content, length and channel. All of them are configurable at runtime offering a very versatile and flexible application for testing.
The packets and the sequence can be modified with the predefined CLI commands.

### CLI commands ###

- help: List the available CLI commands.
- setPayload: Set the packet's payload bytes for future transmits.\
*[uint8] packetID*\
*[uint16] offset*\
*[uint8opt] byte0 byte1 ...*
- getPayload: Print the packet's payload data and byte length.\
*[uint8] packetID*
- setLength: Set the packet's length.\
*[uint8] packetID*
- setChannel: Set packet's TX channel.\
*[uint8] packetID*
- getPacketInfo: Print packet's info.\
*[uint8] packetID*
- setSeq: Set the TX sequence.\
*[uint32opt] packetID0 delayUs0 payketID1 delayUs1*
- getSeq: Print the current TX sequence.
- setRepeat: Set sequence repeat time. 0 means infinite repeat time.\
*[uint32] repeat*
- getRepeat: Print sequence repeat time.
- tx: Start/Stop sequence TX.
- txLog: Enable TX printing messages.\
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

### Command Scripting ###

The RF Generator also supports command scripting the same way as [RAILtest does](https://www.silabs.com/documents/public/user-guides/ug409-railtest-users-guide.pdf#page=6).

### Testing ###

On startup the **RF Generator (TX) node** will be displayed:

```
> RF Generator
```

The **Receiver (RX) node** will be displayed:

```
> Simple TRX
```

The devices start in receive state.

On the **RF Generator (TX) node** issue the `tx` command on the TX node to request/stop packet transmission:

```
tx
> Packet 0 has been sent
Packet 1 has been sent
Packet 2 has been sent
Packet 3 has been sent
tx
Tx aborted
```

On the receiver side the following response will be printed out on packet
reception:

```
> Packet has been received: 0x0F, 0x0E, 0x11, 0x22, 0x33, 0x44, 0x55, 0x0F, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE
Packet has been received: 0x0F, 0x0E, 0x11, 0x22, 0x33, 0x44, 0x55, 0x0F, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE
Packet has been received: 0x0F, 0x0E, 0x11, 0x22, 0x33, 0x44, 0x55, 0x0F, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE
Packet has been received: 0x0F, 0x0E, 0x11, 0x22, 0x33, 0x44, 0x55, 0x0F, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE
```

**Note:** You can press the Button 0 and the Button 1 to request/stop packet transmission respectively.

## .sls Projects Used ##

Project | Use
-|-|
[rail_rf_generator.sls](SimplicityStudio/rail_rf_generator.sls) | RF Generator (TX) node
[rail_soc_simple_trx.sls](SimplicityStudio/rail_soc_simple_trx.sls) | Receiver (RX) node

## How to Port to Another Part ##

- Import the .sls file into Simplicity Studio
- Open the .slcp file of each project, turn to "Overview" tab, hit button "Change Target/SDK", then select the board and part.
