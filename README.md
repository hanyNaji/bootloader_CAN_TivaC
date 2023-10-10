# Bootloader via CAN in Tiva C TM4C123

This project implements a bootloader via CAN communication on the Tiva C TM4C123 microcontroller platform. The bootloader allows for the remote firmware update of the microcontroller, making it a crucial component for embedded systems requiring over-the-air updates.

## Project Overview

The project consists of two parts: the First Electronic Control Unit (ECU) and the Second ECU.

### First ECU

- **Description:** The First ECU is responsible for sending firmware updates over the CAN bus. It listens for a specific trigger signal and sends the firmware data to the Second ECU when the trigger is detected.

- **Features:**
  - Detects a trigger signal on GPIO_PIN_4 to initiate the firmware update.
  - Sends firmware data over CAN when the trigger is detected.

### Second ECU

- **Description:** The Second ECU is responsible for receiving the firmware updates over the CAN bus. It waits for the incoming firmware data and stores it in memory for future use.

- **Features:**
  - Receives firmware data via CAN.
  - Stores the received firmware in memory.
  - Jumps to the bootloader to update the firmware.


## Prerequisites

- Tiva C TM4C123 microcontroller boards for both the First and Second ECUs.
- Code Composer Studio or other compatible development environment.
- 2 CAN Tranceivers.

## Installation and Usage

1. Clone this repository to your local development environment.
2. Set up your development environment with the required tools and libraries.
3. Open the respective ECU project in your IDE (First or Second ECU).
4. Build and flash the code onto the Tiva C TM4C123 boards for each ECU.
5. Ensure that the ECUs are properly connected and the CAN bus is set up.
6. Initiate the trigger signal on the First ECU to send firmware updates.
7. The Second ECU will receive and store the firmware updates.
8. Implement the necessary bootloader to update the firmware from the stored data.
