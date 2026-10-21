# ARM-Based Real-Time Currency Screening System

An embedded systems project using the **STM32F411CEU6 ARM Cortex-M4 microcontroller** and optical sensors to analyze currency-note characteristics and indicate whether a sample is consistent with predefined reference readings.

## Project Overview

The system reads RGB color information from a TCS34725 color sensor and uses an infrared (IR) sensor to detect the presence of a sample. The STM32 processes the sensor readings and indicates the result using an RGB LED.

## Features

- STM32F411CEU6 microcontroller
- TCS34725 RGB color sensing over I²C
- IR sensor input for sample detection
- RGB LED status indication
- Embedded C firmware using STM32 HAL
- STM32CubeIDE and STM32CubeMX development workflow

## Hardware Requirements

- STM32F411CEU6 development board
- TCS34725 RGB color sensor module
- IR sensor module
- Common-cathode RGB LED
- Current-limiting resistors for the LED
- Suitable jumper wires and USB/ST-LINK programmer

## Pin Connections

| Component | Signal | STM32F411CEU6 Pin |
|---|---|---|
| TCS34725 | SCL | PB6 |
| TCS34725 | SDA | PB7 |
| IR sensor | Digital output | PA1 |
| RGB LED | Red | PA5 |
| RGB LED | Green | PA6 |
| RGB LED | Blue | PA7 |

## Software and Tools

- C programming language
- STM32CubeIDE
- STM32CubeMX
- STM32 HAL drivers
- Git and GitHub

## Working Principle

1. The IR sensor detects the presence of a sample.
2. The TCS34725 measures red, green, blue, and clear-channel light values.
3. The STM32 reads the sensor data over I²C.
4. The firmware compares the readings against the configured reference criteria.
5. The RGB LED indicates the firmware's classification result.

The classification depends on the reference values and thresholds configured in the firmware. Performance may vary with lighting, sensor positioning, note condition, and other environmental factors.

## Results

- The system reads sensor data using the STM32 microcontroller.
- The programmed logic processes the sensor readings.
- RGB LED indicators provide visual feedback based on the detection result.

The project demonstrates a basic sensor-based currency screening prototype.

## Project Structure

CounterfeitDetector_Project/
├── Core/
│   ├── Inc/
│   └── Src/
├── Drivers/
│   ├── CMSIS/
│   └── STM32F4xx_HAL_Driver/
├── CounterfeitDetector_Project.ioc
├── .project
├── .cproject
├── .mxproject
├── STM32F411CEUX_FLASH.ld
├── STM32F411CEUX_RAM.ld
├── .gitignore
└── README.md

## Build and Flash

1. Install STM32CubeIDE.
2. Clone or download this repository.
3. Open STM32CubeIDE and import the project as an existing STM32 project.
4. Check the `.ioc` configuration and confirm the sensor pin assignments.
5. Build the project.
6. Connect the STM32F411CEU6 board through ST-LINK.
7. Flash the firmware and test the sensors and LED indicators.

## Author

**Yogavelan M D**
