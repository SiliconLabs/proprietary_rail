# IQ Sampling #

## Overview ##

IQ samples are a fundamental representation of a signal in the form of its
In-phase (I) and Quadrature-phase (Q) components, typically used in
software-defined radios and digital signal processing. This example application
serves as a demonstration of collecting IQ samples with RAIL.

## Gecko SDK version ##

GSDK v4.3.0

## Hardware Required ##

- 1 x WSTK board
- 1 x BRD4210A

## Connections Required ##

- Connect the radio board to the WSTK mainboard.

- Connect the WSTK mainboard to the PC through a USB cable.

## Setup ##

To set up the application, import the .sls project file to Simplicity Studio 5.
Compile the project and download the application to your radio board. This
project can also be created from a `RAIL - SoC Empty` example:

- Create the `RAIL - SoC Empty` example for your board.
- Copy all attached files in the inc and src folders into the project root
  folder.
- Install the following Software Components:
  - `IO Stream: USART` or `IO Stream: EUSART`
  - `IO STREAM: Retarget STDIO`
  - `Assert`
  - `Simple Button` with `btn0` instance.
- If you are using WSTK, enable `Virtual COM UART` in the `Board Control`
  component.
- `GPIO Init` with `sampling_time` instance, select a pin that you want to
  measure the sampling time with.
- Change the PHY in the Radio Configurator to one with a lower (e.g. 50kbps)
  bitrate.

## How it Works ##

RAIL's support for IQ capture is limited to continuous mode, where the radio
constantly records IQ samples and collects them in a designated buffer as long
as its in recieve state or the buffer owerflows. During this mode, it cannot
receive packets.

To prevent the RX FIFO from overflowing, it is crucial for the application to
read out and reset the FIFO regularly, particularly when it nears its maximum
capacity, that is when the `RAIL_EVENT_RX_FIFO_ALMOST_FULL` fires.

You can modify the number of samples collected by adjusting the `RX_FIFO_SIZE`
macro. Note that this controls the size of the buffer storing the IQ-pair
samples, with each element being 2 bytes wide, which means the net IQ-pair
sample count will be a quarter of this value (4 bytes/sample).

RAIL provides [multiple
forms](https://community.silabs.com/s/article/rail-tutorial-special-data-modes?language=en_US#:~:text=Special%20data%20sources)
of data sources to choose from to populate its internal RX FIFO, and IQ data is
one of them. In hardware, the IQ samples are stored in 19 bits. You can select
whether to fetch the upper 16 bits (`RX_IQDATA_FILTMSB`) or the lower 16 bits
(`RX_IQDATA_FILTLSB`) using the `RX_DATA_SOURCE` macro.

> **_Note:_** The order of the samples in RX_IQDATA_FILTLSB is I, Q, while
> RX_IQDATA_FILTMSB presents them in the opposite sequence.

### Sampling Frequency ###

The sample rate is PHY-dependent but can be easily measured by toggling a GPIO
pin upon the `RAIL_EVENT_RX_FIFO_ALMOST_FULL` events. Between two such events,
`RX_FIFO_THRESHOLD`/4 IQ-pair samples are collected from which the sampling
frequency can be easily calculated:

$fs = RX\_FIFO\_THRESHOLD/(4*T_t)$

where $T_t$ is the time between two consecutive `RAIL_EVENT_RX_FIFO_ALMOST_FULL`
events.

## Testing ##

Establish the serial connection with the device and open a terminal. Pressing
PB0 initiates IQ sampling and populates the RX FIFO with the collected data.
After the data is collected, the IQ samples are printed on the serial terminal.
