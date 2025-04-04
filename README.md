## Introduction

This repository provides an integration of Infineon's ModusToolbox™ Machine Learning (MTBML) flow with Avnet's IoTConnect platform, demonstrating a fall detection model. The application utilizes data from the on-board inertial measurement unit (IMU) to detect fall events and reports them to IoTConnect.

For instructions on modifying and using this application template to create applications, follow steps from the "My first PSoC&trade; 6 MCU design using ModusToolbox&trade;" section in [AN228571](https://www.infineon.com/an228571) – Getting started with PSoC&trade; 6 MCU on ModusToolbox&trade; software or the "My first XMC7000 MCU design using Eclipse IDE for ModusToolbox&trade;" section in [AN234334](https://www.infineon.com/dgdl/Infineon-AN234334_Getting_started_with_XMC7000_MCU_on_ModusToolbox_software-ApplicationNotes-v01_00-EN.pdf?fileId=8ac78c8c8412f8d301842d32c5765bfd) – Getting started with XMC7000 MCU on ModusToolbox&trade; software.

The project supports Eclipse and VSCode with GCC_ARM compiler.

## QuickStart Guide

To quickly evaluate this project without compiling code follow the step-by-step instructions in the [QuickStart Guide](QUICKSTART.md).

## Supported Toolchains (make variable 'TOOLCHAIN')

* GNU Arm® Embedded Compiler (GCC_ARM) - Default value of TOOLCHAIN

## Supported Boards

The code has been developed and tested with MTB 3.3, with VsCode, and the board(s) below:

### Supported kits (make variable 'TARGET')

- [PSoC&trade; 6 AI Evaluation Kit](https://www.infineon.com/CY8CKIT-062S2-AI) (`CY8CKIT-062S2-AI`)

## Building the Project

[Watch an overview video](https://saleshosted.z13.web.core.windows.net/media/ifx/videos/IFX%20Modus%20with%20IoTConnect.mp4) of creating a new project with /IOTCONNECT in ModusToolbox&trade; then follow the steps below.

To build the project, please refer to the 
[/IOTCONNECT ModusToolbox&trade; Basic Sample Developer Guide](https://github.com/avnet-iotconnect/avnet-iotc-mtb-basic-example/tree/release-v7.0.1/DEVELOPER_GUIDE.md) 
and note the following:
- Once ModusToolbox has been installed, the [ModusToolbox&trade; for Machine Learning](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolboxpackmachinelearning) software should be installed as well.
- Over-the-air updates are not currently supported.
- Use the [psoc6airm-device-template.json Device Template](https://raw.githubusercontent.com/avnet-iotconnect/avnet-iotc-mtb-ai-baby-monitor/main/files/psoc6airm-device-template.json) instead of the Basic Sample's template.
  **Note:** Right-click the link and select "Save Link As" to download the file.

## Running the Demo
The DEEPCRAFT™ model is tailored to detect dangerous falls among elderly people and performs best on unpadded falls. For optimal results, it's recommended to test with a healthy adult and use pillows to avoid injury. The model is designed with very low false positives per user per week, provided the person lays still for 7-10 seconds after the fall event.

### IMU Orientation
Ensure the IMU (Inertial Measurement Unit) orientation matches the accelerometer X, Y, Z axes:

- **Bird’s eye view**: Arm flat on table
- **Y = 1, X = 0, Z = 0**: Hand held up
- **X = -1, Y = 0, Z = 0**: Hand outstretched, palm facing front
- **Z = -1, X = 1, Y = 0**: Hand outstretched, palm up
![PSOC6AIKit Orientation](http://saleshosted.z13.web.core.windows.net/demo/ifx/deepcraft/psoc6ai-kit-mounting.jpg)

## /IOTCONNECT Device Rules
Use /IOTCONNECT rules to manage notifications and responses:

- **Rules List**: Configure rules to handle specific incidents, such as "Fall Detected" or "Device Disconnected."
- **Notification Setup**: Customize notifications via email, push notifications, webhook, or UI alerts based on triggered events.

To make it easier to observe instant gesture detections in a user interface, the last gesture will "linger"
for some time even when no detection occurs. This application behavior can be controlled with the
*set-linger-interval* command (see commands below). 

One can also supply a shorter interval (eg. 500 ms) as a parameter to the **set-reporting-interval** command 
at runtime to increase the reporting interval. this is especially useful for gesture detections.

The following commands can be sent to the device using the /IOTCONNECT Web UI:

| Command                  | Argument Type     | Description                                                                                                                                                                 |
|:-------------------------|-------------------|:----------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `board-user-led`         | String (on/off)   | Turn the board LED on or off                                                                                                                                                |
| `set-reporting-interval` | Number (eg. 4000) | Set telemetry reporting interval in milliseconds.  By default, the application will report gestures every 1000ms and Audio every 2500ms                                     |
| `set-linger-interval`    | Number (eg. 4000) | Set linger interval in milliseconds. By default, the gestures will linger for 5 seconds and audio detection will not linger. Set to 1 if you wish to disable this behavior. |
| `demo-mode`              | String (on/off)   | Enable demo mode. In this mode the application will send telemetry to /IOTCONNECT for a longer period                                                                        |

## Other /IOTCONNECT-enabled Infineon Kits
See the list [here](https://avnet-iotconnect.github.io/#infineon-technologies)
