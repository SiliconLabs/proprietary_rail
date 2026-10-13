# Range Test SD Log #

## Overview ##

This service extends the RAIL - SoC Range Test application with measurement
data logging to an SD card and an optional GPS positioning interface. It relies
on user-implemented drivers for the required peripherals.

## Configuration ##

The following component settings can be configured in
`sl_range_test_sd_log_config.h`:

#### `SL_GPS_DRIVER_PRESENT` ####

Enables the GPS driver interface.

#### `SL_RANGE_TEST_SD_LOG_BUFFER_SIZE` ####

Sets the SD logging buffer size in bytes.

The following log format definitions are declared in `range_test_sd_log.h`.
This header is part of the driver interface and is not normally meant to be
edited by the user:

#### `SL_RANGE_TEST_SD_LOG_HEADER_RX_GPS` ####

Defines the Rx log header used when GPS is present.

#### `SL_RANGE_TEST_SD_LOG_HEADER_TX_GPS` ####

Defines the Tx log header used when GPS is present.

#### `SL_RANGE_TEST_SD_LOG_HEADER_RX_NO_GPS` ####

Defines the Rx log header used when GPS is not present.

#### `SL_RANGE_TEST_SD_LOG_HEADER_TX_NO_GPS` ####

Defines the Tx log header used when GPS is not present.

#### `SL_RANGE_TEST_SD_LOG_SETTINGS_HEADER_RX` ####

Defines the Rx log settings header.

#### `SL_RANGE_TEST_SD_LOG_SETTINGS_HEADER_TX` ####

Defines the Tx log settings header.

#### `SL_RANGE_TEST_SD_LOG_SEPARATOR` ####

Defines the log separator.
