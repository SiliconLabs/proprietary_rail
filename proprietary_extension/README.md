# Proprietary Extension #

This repository subtree contains a Simplicity SDK extension intended to host
proprietary-oriented components, drivers, and example applications demonstrating
how to bring up and use content stored here.

The extension is intentionally broader than any single driver family. It can
contain content related to legacy proprietary radios, RAIL-adjacent utilities,
and other components published alongside the main repository over time.

----

Useful background documentation:

- [Install SDK extensions](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-getting-started/install-sdk-extensions)
- [SLC CLI user guide](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-tools-slc-cli/)
- [Example project workflow in Simplicity Studio](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#example-projects-demos-tab)

## SDK Compatibility ##

This extension declares the following SDK metadata:

- Extension ID: `proprietary_extension`
- Extension version: `1.0.0`
- Target SDK: `simplicity_sdk` `2025.6.0`
- Vendor: `silabs`

The currently included example metadata targets:

- Device Type: `SoC`
- MCU: `32-bit MCU`
- Quality: `experimental`

Current components and examples in this extension are also marked
`experimental`.

## Examples ##

This extension provides the following example projects:

| No  | Name                                           | Example                        | Documentation                                  |
| --- | ---------------------------------------------- | ------------------------------ | ---------------------------------------------- |
| 1   | Proprietary Extension - EZRadio Driver - Empty | [Example](./app/ezradio_empty) | [Documentation](./app/ezradio_empty/README.md) |

## Components ##

This extension provides the following components.

| No  | Component  | Path                                                                   | Notes                                                                                                  |
| --- | ---------- | ---------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------ |
| 1   | EZRADIODRV | [driver/component/ezradiodrv.slcc](./driver/component/ezradiodrv.slcc) | Driver-specific details are documented in [driver/ezradiodrv/README.md](./driver/ezradiodrv/README.md) |
| 2   | Si4455     | [driver/component/si4455.slcc](./driver/component/si4455.slcc)         | EZRadio part component                                                                                 |
| 3   | Si4460     | [driver/component/si4460.slcc](./driver/component/si4460.slcc)         | EZRadioPRO part component                                                                              |
| 4   | Si4461     | [driver/component/si4461.slcc](./driver/component/si4461.slcc)         | EZRadioPRO part component                                                                              |
| 5   | Si4463     | [driver/component/si4463.slcc](./driver/component/si4463.slcc)         | EZRadioPRO part component                                                                              |
| 6   | Si4467/68  | [driver/component/si4468.slcc](./driver/component/si4468.slcc)         | EZRadioPRO part component                                                                              |

## Repository Layout ##

- [app](./app): example applications packaged by the extension
- [driver/component](./driver/component): extension component metadata used by
  Simplicity SDK / SLC
- [driver](./driver): driver implementations packaged by the extension

Driver-specific documentation should live with the corresponding driver subtree.
For the currently included EZRADIODRV content, see
[driver/ezradiodrv/README.md](./driver/ezradiodrv/README.md).

## Using Extension Content ##

The typical flow for any content in this extension is:

1. Install and trust the extension in the target SDK.
2. Create a project from one of the included examples, or create your own SoC
   project and add extension components with Simplicity Studio or `slc`.
3. Follow the README shipped with the specific driver or example you are using.

### Installation with Simplicity Studio ###

> **Detailed documentation available on [https://docs.silabs.com](https://docs.silabs.com/application-examples/latest/ae-getting-started/how-do-you-use-it#adding-sdk-extensions-for-hardware-drivers)**.

First, clone the repository somewhere on your PC

```
git clone https://github.com/SiliconLabs/proprietary_rail.git`
```

Then add the SDK extension to the target Simplicity SDK.

1. In Simplicity Studio go to **Preferences** → **Simplicity Studio** → **SDKs** and select the Simplicity SDK Suite to which the SDK extension will be added. Click **Add Extension…**
2. Click **Browse** and navigate to the proprietary_extension folder and click **Select Folder**.
3. The SDK extension should be displayed in the Detected SDK Extension window with the **Proprietary Extension** name, version, and path. Click **OK** and then **Trust** and **Apply and Close**.
4. Restart Simplicity Studio.
5. Create a project from one of the included examples or add extension components to an existing SoC project.
6. Follow the README in the specific driver or example subtree for content-specific setup and usage.

> Make sure that **Evaluation** quality software components are enabled in the Software Component view.

For general extension installation behavior, see the Silicon Labs
[SDK extension documentation](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-getting-started/install-sdk-extensions).

### Installation and Usage with SLC-CLI Only ###

First, clone the repository somewhere on your PC.

```
git clone https://github.com/SiliconLabs/proprietary_rail.git`
```

1. Copy `proprietary_extension` under the SDK `extension` folder:

```text
{sdk_root}/extension/proprietary_extension
```

2. Trust the extension signature with `slc`.

Generic form:

```bash
slc signature trust --sdk={sdk_root} -extpath=extension/proprietary_extension
```

3. Use `slc` commands against that SDK to generate or inspect projects that use
   the extension.
4. Refer to the README located in the specific driver or example subtree for
   content-specific usage instructions.

For SLC CLI usage details, see the Silicon Labs
[SLC CLI documentation](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-tools-slc-cli/).
