# CPU Support package for the iMXRT-Family from NXP

This CPU Support package enables the development for the iMXRT microcontroller family using the [Crossworks development environment](https://rowley.co.uk/arm/index.htm) from Rowley. The package provides all needed data and information to enabled the Development Environment to compile and debug applications on the iMXRT microcontroller
This Includes:

- CMSIS-Specific Code Files
- Memory-Map-Files
- Register-Descriptions for debugging
- Loader for programming the application to the Flash-memory

The Package is based on the [work from Rowley](https://www.rowleydownload.co.uk/arm/packages/iMXRT.htm). The original versions are included in the git-History and are tagged with a Rowley label.
This package enhances the original package by

- Using the latest CMSIS-Files and register-descriptions provided by NXP
- Enabling debugging in the external-SDRAM-memory
- Added an Open-Source loader which supports different Flash memory targets
- Added sample code for booting from external Flash
- Adding Support for iMXRT1010-Family

The following Controllers are supported:

- [iMXRT1010](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/i.mx-rt-series/i.mx-rt1010-crossover-mcu-with-arm-cortex-m7-core:i.MX-RT1010)
- [iMXRT1015](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/i.mx-rt-series/i.mx-rt1015-crossover-processor-with-arm-cortex-m7-core:i.MX-RT1015)
- [iMXRT1020](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/i.mx-rt-series/i.mx-rt1020-crossover-processor-with-arm-cortex-m7-core:i.MX-RT1020)
- [iMXRT1050](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/i.mx-rt-series/i.mx-rt1050-crossover-processor-with-arm-cortex-m7-core:i.MX-RT1050)
- [iMXRT1060](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/i.mx-rt-series/i.mx-rt1060-crossover-processor-with-arm-cortex-m7-core:i.MX-RT1060)
- [iMXRT1064](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/i.mx-rt-series/i.mx-rt1064-crossover-processor-with-arm-cortex-m7-core:i.MX-RT1064)

The Support for [iMXRT1170](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/i.mx-rt-series/i.mx-rt1170-crossover-mcu-family-first-ghz-mcu-with-arm-cortex-m7-and-cortex-m4-cores:i.MX-RT1170) is in development.

## Build and Install

The Package is provided unpacked and has to be packed before it can be installed. For this open the Project File located in the root of the project with the Crossworks IDE (File imxrt.hzp). Then “compile" it like a normal project (Build -> Build iMXRT).This will create the CPU-Support package in the root-Folder of the project (-> iMXRT.hzq).
To install the new created package go to Tools -> Packages -> Manually Install Packages and choose the new File.
![Package Manager](./doc/Menu_PackageManagerManual.png)
Attention! Uninstall a possibly already existing iMXRT-CPU-Support package before installing the new one. The Folder Structure and the predefined macros are different, those packages are not compatible to each other.
