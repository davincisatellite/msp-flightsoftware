# Setup for Software Development at Da Vinci Satellite
This document provides a step‑by‑step guide for installing and configuring the required software tools to develop, build, and flash flight software for the MSP432P401R microcontroller used in the Da Vinci Satellite student team. It covers installation of Code Composer Studio (CCS), the SimpleLink MSP432 Software Development Kit, GNU Arm Embedded Toolchain, and TI Arm Compiler, along with environment setup and verification steps.

This guide assumes a Windows operating system, and you need to have at least 12 GB free disk space.

---
- Install the latest version of Code Composer Studio (CCS).
    - Go to the link [Code Composer Studio](https://www.ti.com/tool/download/CCSTUDIO)
    - Download the Windows installer
    - Extract the zip somewhere in your computer (doesn't matter where)
    - Run the setup executable
    - Accept the user agreement stuff, click next
    - Read the info on the new page, and click next
    - Come to the installation directory page, accept the default directory, which will likely be `C:\ti\ccs2031`, and click next
    - For the components you want to install, select the following:
        - Arm-based processors
        - MSP430 microcontrollers
        - SimpleLink MSP432 low power + performance MCUs
    - Proceed to install (can take ~10 minutes or more)
    - Click finish at the end of installation with at least the `Create Desktop Shortcut` option toggled on

- Install the MSP432 Software Development Kit (SDK)
    - Go to the link [SIMPLELINK-MSP432-SDK](https://www.ti.com/tool/download/SIMPLELINK-MSP432-SDK/3.40.01.02)
    - Download the Windows installer
    - In the 'Installation Directory' page, accept the default path, which should be `C:\ti`
    - Proceed to install (can take ~5 minutes or more)
    - Click finish at the end of installation. If you want to check some documentation and guide, you can leave the 2 options toggled on

- Install the GNU Arm Embedded Toolchain
    - Use the link [GNU Arm Embedded Toolchain](https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2017q4/gcc-arm-none-eabi-7-2017-q4-major-linux.tar.bz2?rev=375265d4e9b541c8bf2356cbe927e156&revision=375265d4-e9b5-41c8-bf23-56cbe927e156?product=Downloads,64-bit,,Linux,7-2017-q4-major)
        > **Note:** This link won't direct you to a page, but start downloading a compressed archive file with an extension *.tar.bz2

    > **Note:** You need to have Windows Subsystem for Linux (WSL) installed on your computer for the next step. The only reason is that we need to decompress that *.tar.bz2 file, and I only knew how to do it on Linux. If you know another way that doesn't require WSL, feel free to use that method.
    - Open Ubuntu terminal (or whatever distribution of Linux you have), and run the following commands:
        - `mkdir gcc-arm`
        - `cd gcc-arm/`
        - `sudo tar -xvf /mnt/c/Users/BoraMenekse/Downloads/gcc-arm-none-eabi-7-2017-q4-major-linux.tar.bz2 -C .`
            > **Note:** Mind the user name in the path to the *.tar.bz2 file. It will be different for everyone, please modify the path accordingly to where you downloaded the file
        - Enter your Linux user account password
        - run `ls` to list what is inside the directory, and you should see a folder with name `gcc-arm-none-eabi-7-2017-q4-major`
        - run `cp -r gcc-arm-none-eabi-7-2017-q4-major /mnt/c/ti/` to copy the whole folder to your `ti` folder in the C drive, where you installed other things

- Go to the SimpleLink SDK installation directory, open and modify the `imports.mak` file:
    - If you couldn't find it, the installation directory should be `C:\ti\simplelink_msp432p4_sdk_3_40_01_02`
        ```
        #XDC_INSTALL_DIR        ?= c:/ti/xdctools_3_60_02_34_core
        #SYSCONFIG_TOOL         ?= c:/ti/ccs930/ccs/utils/sysconfig/sysconfig_cli.bat

        #FREERTOS_INSTALL_DIR   ?= c:/FreeRTOSv10.1.1

        #CCS_ARMCOMPILER        ?= c:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS
        GCC_ARMCOMPILER        ?= c:/ti/gcc-arm-none-eabi-7-2017-q4-major
        #IAR_ARMCOMPILER        ?= c:/Program Files (x86)/IAR Systems/Embedded Workbench 8.2/arm
        ```
    - The point is to comment out all the variables seen above, except `GCC_ARMCOMPILER`, and modify its value to where you copied the `gcc-arm-none-eabi-7-2017-q4-major` folder
    - Keep the rest of the file as is

- Install the TI Arm Compiler
    - Go to the link [ARM-CGT Compiler](https://www.ti.com/tool/download/ARM-CGT/20.2.7.LTS)
    - Download the Windows Installer
    - Select the following installation directory: `C:\ti\ccs2031\ccs\tools\compiler\ti-cgt-arm_20.2.7.LTS`
        > **Note:** Depending on the CCS version, the folder may not be ccs2031 for you, but something else, adjust accordingly please. It's ccs2031 for me, because I installed the version 20.3.1 of CCS.

- Set a system environment variable on your computer:
    - Name: `SIMPLELINK_MSP432_SDK_INSTALL_DIR`
    - Value: `C:\ti\simplelink_msp432p4_sdk_3_40_01_02`

- Some verification tests to ensure CCS recognizes the installed compiler and SDK before you attempt to build projects:
    - Open CCS, and go to `Settings` at bottom left
    - Open `Code Composer Studio Settings`
    - Under `General > Compilers`, check whether you have `TI v20.2.7.LTS : C:\ti\ccs2031\ccs\tools\compiler\ti-cgt-arm_20.2.7.LTS` under the `Discovered compilers` section. If not, try refreshing first using the `Refresh` button on the right side in the same line as the `Discovered compilers` text.
    - Under `General > Products`, check whether you have `SimpleLink MSP432P4 SDK` under the `Discovered products` section. If not, try refreshing the same way as before.
---

### ✅ **Setup is completed at this point.**

- In order to build the flight software project and test it on MSP432P401R, here are some additional steps:
    - Clone the [msp-flightsoftware](https://github.com/davincisatellite/msp-flightsoftware.git) repository
    - Check out to a branch you like
    - Open CCS, and open the project folder (msp-flightsoftware) via `File > Open Folder`
    - Use either the keyboard shortcut `Ctrl+B` to build the project, or go to `Project` in the top menu bar and click on `Build Projects`
    - A file with an extension `*.out` should be created, if the build succeeds.
    - In order to flash/upload the created application to the microcontroller, go to `Run` in the top menu bar, and click on `Flash Project`. You need your computer connected to the microcontroller with a micro-USB cable for this. You will see some LEDs blinking (red and/or green) to indicate flashing has succeeded.
