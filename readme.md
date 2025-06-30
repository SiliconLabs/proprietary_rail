<table border="0">
  <tr>
    <td align="left" valign="middle">
    <h1>EFR32 RAIL Application Examples</h1>
  </td>
  <td align="left" valign="middle">
    <a href="https://www.silabs.com/wireless/proprietary">
      <img src="http://pages.silabs.com/rs/634-SLU-379/images/WGX-transparent.png"  title="Silicon Labs Gecko and Wireless Gecko MCUs" alt="EFM32 32-bit Microcontrollers" width="250"/>
    </a>
  </td>
  </tr>
</table>

# Silicon Labs RAIL #

Silicon Labs RAIL provides an intuitive, easily-customizable radio interface layer designed to support proprietary or standards-based wireless protocols. RAIL is delivered as a library that you can link to your applications. RAIL implements commonly-used radio functionality so that it does not have to be written in the application or stack, eliminating the need for developers to become experts in RF register details of complex wireless SoCs. This functionality includes radio configuration, transmit, receive, clear channel assessment, automatic states transitions, auto-ACK, system timing, and channel hopping. All these features can be used to build fully customized network stacks on top of the RAIL API.

## Examples ##

| Example name                                     |                  Link to example                   |
|:-------------------------------------------------|:--------------------------------------------------:|
| RAIL Proprietary - Time Synchronization          |     [Click Here](./rail_time_synchronization)      |
| RAIL Proprietary - Tutorial Transmit             |       [Click Here](./rail_tutorial_transmit)       |
| RAIL Proprietary - Tutorial Event Handling       |    [Click Here](./rail_tutorial_event_handling)    |
| RAIL Proprietary - Tutorial Downloading Messages | [Click Here](./rail_tutorial_downloading_messages) |
| RAIL Proprietary - Direct Mode Packet Detector   |     [Click Here](./rail_direct_mode_detector)      |
| RAIL Proprietary - Direct to Buffer              |       [Click Here](./rail_direct_to_buffer)        |
| RAIL Proprietary - Address Filtering             |       [Click Here](./rail_address_filtering)       |
| RAIL Proprietary - Improved RF Sense Transmitter | [Click Here](./rail_improved_rfsense_transmitter)  |
| RAIL Proprietary - RF Generator                  |         [Click Here](./rail_rf_generator)          |
| RAIL Proprietary - State Transition Test         |     [Click Here](./rail_state_transition_test)     |

## Documentation ##

Official documentation can be found at our [Developer Documentation](https://docs.silabs.com/rail/latest/) page.

## Working with Projects ##

1. To add an external repository, perform the following steps.

    - From Simpilicity Studio 5, go to **Preferences > Simplicity Studio > External Repos**. Here you can add the repo `https://github.com/SiliconLabs/proprietary_rail`.

    - Cloning and then selecting the branch, tag, or commit to add. The default branch is Master. This repo cloned to `<path_to_the_SimplicityStudio_v5>/developer/repos/`

2. From Launcher, select your device from the "Debug Adapters" on the left before creating a project. Then click the **EXAMPLE PROJECTS & DEMOS** tab -> check **proprietary_rail** under **Provider** to show a list of Proprietary RAIL example projects compatible with the selected device. Click CREATE on a project to generate a new application from the selected template.

Alternatively the example application can be created directly by SLC-CLI invocations as documented in the [SLC-CLI: Usage](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-tools-slc-cli/03-usage) page.

For further information on how to work with projects, please refer to the [Application Examples: Usage](https://docs.silabs.com/application-examples/latest/ae-getting-started/how-do-you-use-it) page.

## Reporting Bugs/Issues and Posting Questions and Comments ##

To report bugs in the Application Examples projects, please create a new "Issue" in the "Issues" section of this repo. Please reference the board, project, and source files associated with the bug, and reference line numbers. If you are proposing a fix, also include information on the proposed fix. Since these examples are provided as-is, there is no guarantee that these examples will be updated to fix these issues.

Questions and comments related to these examples should be made by creating a new "Issue" in the "Issues" section of this repo.

## Disclaimer ##

The Gecko SDK suite supports development with Silicon Labs IoT SoC and module devices. Unless otherwise specified in the specific directory, all examples are considered to be EXPERIMENTAL QUALITY which implies that the code provided in the repos has not been formally tested and is provided as-is.  It is not suitable for production environments.  In addition, this code will not be maintained and there may be no bug maintenance planned for these resources. Silicon Labs may update projects from time to time.
