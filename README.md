# RallyOTA

Arduino controller for a Bluetooth Motorcycle Rally Handlebar remote.

## Overview

Features:
 - BLE support to connect to an Android device
 - Over-The-Air updates to modify the software after soldering
 - Two button mappings to alternate between Rally app and OsmAND maps

Hardware list:
 - ESP32 C3 SuperMini
 - 3 x PBS-33B 2PIN 12mm Button
 - 1 x MTS-123 3-pin self-reset micro rocker switch
The remote is connected via USB to an onboard power supply.

## How to use

 - Clone the repository on VSCode
 - Install the PlatformIO extension
 - Connect your board to the computer
 - Build using the [nonim] environment (no Nimble)
 - Connect the serial monitor with baud 115200 to see the logs
 - Modify the consts at the top of [main.cpp](src/main.cpp) to customize the button GPIO pins to your board