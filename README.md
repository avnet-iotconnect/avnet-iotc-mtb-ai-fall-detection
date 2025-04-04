## Introduction

This repository provides an integration of Infineon's ModusToolbox™ Machine Learning (MTBML) flow with Avnet's IoTConnect platform, demonstrating a fall detection model. The application utilizes data from the on-board inertial measurement unit (IMU) to detect fall events and reports them to IoTConnect.

For instructions on modifying and using this application template to create applications, follow steps from the "My first PSoC&trade; 6 MCU design using ModusToolbox&trade;" section in [AN228571](https://www.infineon.com/an228571) – Getting started with PSoC&trade; 6 MCU on ModusToolbox&trade; software or the "My first XMC7000 MCU design using Eclipse IDE for ModusToolbox&trade;" section in [AN234334](https://www.infineon.com/dgdl/Infineon-AN234334_Getting_started_with_XMC7000_MCU_on_ModusToolbox_software-ApplicationNotes-v01_00-EN.pdf?fileId=8ac78c8c8412f8d301842d32c5765bfd) – Getting started with XMC7000 MCU on ModusToolbox&trade; software.

The project supports Eclipse and VSCode with GCC_ARM compiler.

## QuickStart Guide

To quickly evaluate this project without compiling code follow the step-by-step instructions in the [QuickStart Guide](QUICKSTART.md).

## Supported Toolchains (make variable 'TOOLCHAIN')

* GNU Arm® Embedded Compiler (GCC_ARM) - Default value of TOOLCHAIN

## Supported Boards

The code has been developed and tested with MTB 3.3, with VsCode, and the board below:

## Supported kits (make variable 'TARGET')

- [PSoC&trade; 6 AI Evaluation Kit](https://www.infineon.com/CY8CKIT-062S2-AI) (`CY8CKIT-062S2-AI`) – Default value of `TARGET`
- [PSoC&trade; 62S2 Wi-Fi Bluetooth&reg; Pioneer Kit](https://www.infineon.com/CY8CKIT-062S2-43012) (`CY8CKIT-062S2-43012`) **audio models only**

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

For audio models, once the board connects to /IOTCONNECT, it will start processing microphone input and attempt to detect the corresponding sound. 
This can be tested by placing the board in such way so that the microphone close to the PC speaker.
For best results, the microphone should be placed very close and pointed directly towards the speaker.

The following YouTube sound clips can be used for testing (sorted by recognition quality):
  * [Siren](https://www.youtube.com/watch?v=s5bwBS27A1g)
  * [Alarm](https://www.youtube.com/watch?v=hFIJaB6kVzk)
  * [Snore](https://www.youtube.com/watch?v=dXKCapH-vx8)
  * [Cough](https://www.youtube.com/watch?v=Qp09X74kjBc)
  * [Baby Cry](https://www.youtube.com/watch?v=Rwj1_eWltJQ&t=227s)

For radar models, please refer to the [Operation Section](https://github.com/Infineon/mtb-example-ml-imagimob-deploy-ready-model/blob/master/README.md#operation) of the original Infineon's project,
and see the gesture animations at the bottom of the section.
Note that for best detection, the 062S2-AI board **should not be placed on, or near a flat surface**. The surface
behind the board will likely interfere with the sensor. The best way to test gestures on this board
is to hold the board by the USB cable, like shown in the animations. 

When the appropriate sound or gesture is recognized in-between telemetry reporting events,
the *class* telemetry value will be reported as a string with the name of the last detected class (label).

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
See the list [here](https://avnet-iotconnect.github.io/#infineon-technologies)nfineon.com to help you select the right device, and quickly and effectively integrate it into your design.

For PSoC&trade; 6 MCU devices, see [How to design with PSoC&trade; 6 MCU - KBA223067](https://community.infineon.com/docs/DOC-14644) in the Infineon Developer community.


## Document history

Document title: *CE228742* – *Empty application template*

 Version | Description of change
 ------- | ---------------------
 1.0.0   | New code example
 1.1.0   | Updated to support ModusToolbox&trade; software v2.1, added new kits
 2.0.0   | Major update to support ModusToolbox&trade; software v2.2, added support for new kits<br> This version is not backward compatible with ModusToolbox&trade; software v2.1
 2.1.0   | Added support for new kits
 3.0.0   | Updated to BSP v3.X and added support for new kits 
 4.0.0   | Major update to support ModusToolbox&trade; software v3.0 <br> Added support for KIT_XMC72_EVK, CY8CKIT-064S0S2-4343W and CYW920829M2EVB-01
 4.1.0   | Added support for CY8CPROTO-064B0S3 CY8CPROTO-064S1-SB
 4.2.0   | Added support for CY8CEVAL-062S2-LAI-43439M2
 4.3.0   | Removed CYW920829M2EVB-01 from supported ktis <br> Added support for CY8CPROTO-062S2-43439, CYBLE-416045-EVAL and CYW920829M2EVK-02
 4.4.0   | Added support for CY8CEVAL-062S2-MUR-4373EM2, CY8CEVAL-062S2-MUR-4373M2
 4.4.1   | Readme Update
 4.5.0   | Added support for CYW989829M2EVB-01 and updated to support ModusToolbox&trade; v3.1
 4.6.0   | Added support for KIT_XMC72_EVK_MUR_43439M2 and KIT_XMC71_EVK_LITE_V1.
 4.7.0   | Added support for CY8CEVAL-062S2-CYW43022CUB
 4.8.0   | Added support for CY8CKIT-062S2-AI
 4.9.0   | Added support for CY8CEVAL-062S2-CYW955513SDM2WLIPA
 4.9.1   | Added support for devices not supported by HAL, added support for KIT_T2G_C-2D-6M_LITE
<br>

---------------------------------------------------------

© Cypress Semiconductor Corporation, 2020-2024. This document is the property of Cypress Semiconductor Corporation, an Infineon Technologies company, and its affiliates ("Cypress").  This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide.  Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights.  If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress's patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products.  Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.
<br>
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  No computing device can be absolutely secure.  Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product. CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach").  Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach.  In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications. To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document. Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes.  It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product.  "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage.  Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices.  "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness.  Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device. You shall indemnify and hold Cypress, including its affiliates, and its directors, officers, employees, agents, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device. Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress's published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.
<br>
Cypress, the Cypress logo, and combinations thereof, ModusToolbox, PSoC, CAPSENSE, EZ-USB, F-RAM, and TRAVEO are trademarks or registered trademarks of Cypress or a subsidiary of Cypress in the United States or in other countries. For a more complete list of Cypress trademarks, visit www.infineon.com. Other names and brands may be claimed as property of their respective owners.
