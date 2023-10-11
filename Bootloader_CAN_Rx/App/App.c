/*
 * App.c
 *
 *  Created on: Sep 29, 2023
 *      Authors:
 *          - Hany Nagy
 *          - Muhammed Al shafie
 *          - Waleed Gamal
 *          - Hamdy Elsayd
 */

/*Include standard C libraries*/
#include <stdbool.h>
#include <stdint.h>

/* Include TivaWare library header files */
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/flash.h"


/* Include a custom header file */
#include "App.h"




/* Define a macro to call the bootloader function pointer */
#define CALL_BOOTLOADER(address)  \
        (*((void (*)(void))(address)))()

/*
 * A counter that keeps track of the number of times the RX interrupt has
 * occurred, which should match the number of messages that were received.
 */
volatile uint32_t g_ui32MsgCount = 0;

/*
 * A flag for the interrupt handler to indicate that a message was received.
 */
volatile bool g_bRXFlag = 0;

/*
 * A flag to indicate that some reception error occurred.
 */
volatile bool g_bErrFlag = 0;




void system_Config(void)
{
    /* Set the system clock to use a 16MHz external crystal oscillator (main oscillator). */
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    /* Enable the GPIO port that is used for the on-board LED. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    /* Check if the peripheral access is enabled. */
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    /* Enable the interrupts.*/
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD_WPU);

    /* Initialize CAN Communication */
    Can_Init();



}


/* Function to jump to a new program in ROM */
void JumpToBootLoader(uint32_t addr)
{
    /* Disable SysTick and its interrupt */
    SysTickIntDisable();
    SysTickDisable();

    /* Disable all processor interrupts */
    HWREG(NVIC_DIS0) = 0xffffffff;
    HWREG(NVIC_DIS1) = 0xffffffff;

    /* Set the vector table address to the provided address */
    HWREG(NVIC_VTABLE) = addr;

    /* Calculate the reset handler address by adding an offset to the address */
    uint32_t Reset_Handler_newProgram = addr + 0x00001045;

    /* Call the bootloader via the modified function pointer */
    CALL_BOOTLOADER(Reset_Handler_newProgram);
}

/* Function to update firmware in ROM */
void UpdateFirmWare(uint32_t* firmwareData, uint32_t addr, uint16_t firmwareData_Size)
{
    /* Define constants for better readability */
    const uint16_t firmwareBlockSize = 25; /* Size of each block in the firmware */

    uint32_t blockIndex;

    /* Iterate through firmware blocks */
    for (blockIndex = addr; blockIndex < ((1000 * firmwareBlockSize) + addr); blockIndex += 1000)
    {
        /* Erase the current firmware block */
        FlashErase(blockIndex);
    }

    /* Program the new firmware data into ROM */
    FlashProgram(firmwareData, addr, firmwareData_Size);
}





void Can_Init(void) {

    /* Enable the GPIO (General-Purpose Input/Output) peripheral for Port B. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    /* Configure pins PB4 and PB5 to be used for CAN0 receiver (RX) and transmitter (TX). */
    GPIOPinConfigure(GPIO_PB4_CAN0RX);
    GPIOPinConfigure(GPIO_PB5_CAN0TX);

    /* Set the pins PB4 and PB5 as CAN pins for Port B. */
    GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    /* Enable the CAN0 peripheral. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    /* Initialize the CAN0 controller. */
    CANInit(CAN0_BASE);

    /* Set the CAN bit rate to 500,000 bits per second using the current system clock frequency. */
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 500000);

    /* Enable CAN interrupts for the CAN0 controller, including master, error, and status interrupts. */
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    /* Register the CAN interrupt handler function (CANIntHandler) for CAN controller 0 (CAN0_BASE). */
    CANIntRegister(CAN0_BASE, CANIntHandler);

    /* Enable the CAN0 interrupt in the NVIC (Nested Vectored Interrupt Controller). */
    IntEnable(INT_CAN0);

    /* Enable the CAN0 controller. */
    CANEnable(CAN0_BASE);
}

/* Define a function to initialize a CAN message object for transmission */
void CAN_MessageInit(tCANMsgObject* msgObject, uint32_t msgID, uint32_t msgIDMask, uint32_t tx_rx, uint32_t msgLen, uint8_t* ptrData) {
    /* This function takes in a pointer to a CAN message object structure (msgObject) and sets its members. */

    /* Set the 'ui32MsgID' member of the message object (msgObject) to the provided message ID (msgID). */
    msgObject->ui32MsgID = msgID;

    /* Set the 'ui32MsgIDMask' member of the message object (msgObject) to the provided message ID mask (msgIDMask). */
    msgObject->ui32MsgIDMask = msgIDMask;

    /* Set the 'ui32Flags' member of the message object (msgObject) to the provided transmission/reception flag (tx_rx). */
    msgObject->ui32Flags = tx_rx;

    /* Set the 'ui32MsgLen' member of the message object (msgObject) to the provided message length (msgLen). */
    msgObject->ui32MsgLen = msgLen;

    /* Set the 'pui8MsgData' member of the message object (msgObject) to point to the provided message data (ptrData). */
    msgObject->pui8MsgData = ptrData;

    /* In summary, this function takes a message object (msgObject) and initializes its members
     * with the provided message ID, message ID mask, transmission/reception flag, message length, and data pointer.
     * This prepares the message object for sending or receiving a CAN message.
     */
}

void SimpleDelay(void) {
    /* Delay cycles for 1 second */
    SysCtlDelay(16000000 / 8);
}

void CANIntHandler(void) {
    uint32_t ui32Status;

    /* Read the CAN interrupt status to find the cause of the interrupt */
    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    /* If the cause is a controller status interrupt, then get the status */
    if (ui32Status == CAN_INT_INTID_STATUS) {
        /* Read the controller status. This will return a field of status
         * error bits that can indicate various errors. Error processing
         * is not done in this example for simplicity. Refer to the
         * API documentation for details about the error status bits.
         * The act of reading this status will clear the interrupt.
         */
        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);

        /* Set a flag to indicate some errors may have occurred. */
        g_bErrFlag = 1;
    }

    /* Check if the cause is message object 1, which is what we are using for
     * receiving messages.
     */
    else if (ui32Status == 1) {
        /* Getting to this point means that the RX interrupt occurred on
         * message object 1, and the message reception is complete. Clear the
         * message object interrupt.
         */
        CANIntClear(CAN0_BASE, 1);

        /* Increment a counter to keep track of how many messages have been
         * received. In a real application, this could be used to set flags to
         * indicate when a message is received.
         */
        g_ui32MsgCount++;

        /* Set a flag to indicate received message is pending. */
        g_bRXFlag = 1;

        /* Since a message was received, clear any error flags. */
        g_bErrFlag = 0;
    }

    /* Otherwise, something unexpected caused the interrupt. This should
     * never happen.
     */
    else {
        /* Spurious interrupt handling can go here. */
    }
}

void Blue_LED_Toggle(void)
{
    static uint32_t toggle = GPIO_PIN_2;
    toggle ^= GPIO_PIN_2;
    /* toggle the blue led */
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, toggle);
}

void Green_LED_Toggle(void)
{
    static uint32_t toggle = GPIO_PIN_3;
    toggle ^= GPIO_PIN_3;
    /* toggle the blue led */
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, toggle);
}


