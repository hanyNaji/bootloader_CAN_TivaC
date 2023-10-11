# Bootloader via CAN in Tiva C TM4C123

This project implements a bootloader via CAN communication on the Tiva C TM4C123 microcontroller platform. The bootloader allows for the remote firmware update of the microcontroller, making it a crucial component for embedded systems requiring over-the-air updates.


## Transmitter ECU (First ECU)

The Transmitter ECU is responsible for sending firmware updates over the CAN bus. It listens for specific trigger signals on the GPIO pins and sends the firmware data to the Receiver ECU when a trigger is detected.

### Features

- **Two Firmware Variants:** The Transmitter ECU can send two different firmware variants with different lengths: FIRMWARE_1 and FIRMWARE_2.

- **Trigger Signals:** It listens for trigger signals on GPIO_PIN_4 and GPIO_PIN_0. When GPIO_PIN_4 is triggered, it sends FIRMWARE_1. When GPIO_PIN_0 is triggered, it sends FIRMWARE_2.



## Receiver ECU (Second ECU)

The Receiver ECU is responsible for receiving firmware updates over the CAN bus. It waits for incoming firmware data and stores it in memory for future use. Additionally, it can switch between two different firmware variants.

### Features

- **Two Firmware Variants:** The Receiver ECU can receive and store firmware updates of two different variants.

- **Memory Management:** It manages the received firmware data and stores it in memory.


## Prerequisites

- Tiva C TM4C123 microcontroller boards for both the Transmitter and Receiver ECUs.
- Code Composer Studio or other compatible development environment.
- Properly configured GPIO pins for trigger signals.
- Functional CAN bus communication setup.
- Necessary bootloader for firmware updates.

## Installation and Usage

1. Clone this repository to your local development environment.
2. Set up your development environment with the required tools and libraries.
3. Open the respective ECU project in your IDE (Transmitter or Receiver ECU).
4. Build and flash the code onto the Tiva C TM4C123 boards for each ECU.
5. Ensure that the ECUs are properly connected, and the CAN bus is set up.
6. Initiate the trigger signal on the Transmitter ECU to send firmware updates to the Receiver ECU.
7. The Receiver ECU will receive and store the firmware updates.
8. Implement the necessary bootloader to update the firmware from the stored data.

