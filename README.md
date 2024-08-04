# DS18B20 USB Interface with Real-time Plotter

This project implements a USB interface for the DS18B20 temperature sensor using a Raspberry Pi Pico. It includes a Python-based real-time plotter with automatic serial port detection based on VID/PID.

## Table of Contents

1. [Hardware Requirements](#hardware-requirements)
2. [Software Requirements](#software-requirements)
3. [Setup](#setup)
   - [Pico SDK Modification](#pico-sdk-modification)
   - [Pico Firmware](#pico-firmware)
   - [Python Environment](#python-environment)
4. [Usage](#usage)
5. [Project Structure](#project-structure)
6. [Troubleshooting](#troubleshooting)
7. [Contributing](#contributing)
8. [License](#license)

## Hardware Requirements

- Raspberry Pi Pico
- DS18B20 temperature sensor
- Breadboard and jumper wires
- USB cable for Pico connection

## Software Requirements

- Raspberry Pi Pico SDK
- Python 3.7+
- Required Python packages (see `requirements.txt`)

## Setup

### Pico SDK Modification

To change the VID/PID without rewriting the USB serial code:

1. Edit the file: `pico-sdk/src/rp2_common/pico_stdio_usb/stdio_usb_descriptors.c`
2. Modify the following lines:

   ```c
   // Change this line:
   #define USBD_VID (0x2E8A) // Raspberry Pi 
   // To:
   #define USBD_VID (0xf055)

   // Change this line:
   #define USBD_PID (0x000a) // Raspberry Pi Pico SDK CDC
   // To:
   #define USBD_PID (0x0012)
   ```

3. Save the file and rebuild the Pico SDK.

### Pico Firmware

1. Clone this repository.
2. Navigate to the `pico_firmware` directory.
3. Build the firmware using CMake:
   ```
   mkdir build
   cd build
   cmake ..
   make
   ```
4. Flash the resulting `.uf2` file to your Pico.

### Python Environment

1. Create a virtual environment:
   ```
   python -m venv venv
   source venv/bin/activate  # On Windows, use `venv\Scripts\activate`
   ```
2. Install required packages:
   ```
   pip install -r requirements.txt
   ```

## Usage

1. Connect the DS18B20 sensor to your Pico (refer to the wiring diagram in the `docs` folder).
2. Connect the Pico to your computer via USB.
3. Run the plotter script:
   ```
   python temperature_plotter.py
   ```
4. The script will automatically detect the Pico based on the modified VID/PID and start plotting temperature data in real-time.

## Project Structure

```
ds18b20-usb-interface/
├── pico_firmware/
│   ├── CMakeLists.txt
│   └── main.c
├── python_plotter/
│   └── temperature_plotter.py
│   └── requirements.txt
└── README.md
```

## Troubleshooting

- If the plotter can't detect your Pico, ensure that you've correctly modified the Pico SDK and reflashed your device.
- Check the serial output for any error messages.
- Verify that the DS18B20 is correctly wired to the Pico.

