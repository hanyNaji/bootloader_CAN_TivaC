
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/can.h"
#include "inc/hw_can.h"
#include "App/App.h"




#define USE_APP     1  /* 1: first app - 2: second app*/

/*APP 1*/
#if(USE_APP==1)
uint32_t new_APP_Addr = 0x00000000;
uint32_t current_APP_Addr = 0x00020000;
#endif

/*APP 2*/
#if(USE_APP==2)
uint32_t new_APP_Addr = 0x00020000;
uint32_t current_APP_Addr = 0x00000000;
#endif

#define FIRMWARE_LEN    9024

extern volatile bool g_bRXFlag ;
extern volatile uint32_t g_ui32MsgCount ;

tCANMsgObject sCANMessage;
uint8_t pui8MsgData[8];
uint8_t APP1_image_0[FIRMWARE_LEN];


/* Interrupt handler for GPIOF */
void GPIOFIntHandler(void)
{
    /* Clear the interrupt for GPIO_PIN_4 */
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);

    /* Jump to the bootloader using the provided new_APP_Addr */
    JumpToBootLoader(new_APP_Addr);
}


/* Function to receive firmware over CAN */
void CAN_receiveFirmware(void)
{
    g_ui32MsgCount = 0;

    for (;;)
    {
        if (g_bRXFlag)
        {
            /* Calculate the message data pointer for the current message */
            sCANMessage.pui8MsgData = APP1_image_0 + ((g_ui32MsgCount - 1) * 8);

            /* Get a CAN message from CAN0_BASE with object 1 */
            CANMessageGet(CAN0_BASE, 1, &sCANMessage, 0);
            g_bRXFlag = 0;
        }

        if (g_ui32MsgCount - 1 == FIRMWARE_LEN / 8)
        {
            break;
        }
    }

    /* Convert APP1_image_0 to a uint32_t pointer for updating firmware */
    uint32_t *pFirmware = (uint32_t *)APP1_image_0;

    /* Update the firmware using the UpdateFirmWare function */
    UpdateFirmWare(pFirmware, new_APP_Addr, FIRMWARE_LEN);

    /* Clear the GPIO_PORTF_BASE, GPIO_PIN_3 */
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);

    /* Jump to the bootloader using the provided new_APP_Addr */
    JumpToBootLoader(new_APP_Addr);
}


void main(void)
{
    /* Initialize System Configurations */
    system_Config();

    /*
     * Initialize a message object to be used for receiving CAN messages with
     * any CAN ID.  In order to receive any CAN ID, the ID and mask must both
     * be set to 0, and the ID filter enabled.
     */
    sCANMessage.ui32MsgID = 0;
    sCANMessage.ui32MsgIDMask = 0;
    sCANMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    sCANMessage.ui32MsgLen = 8;

    /* Set the message object for receiving */
    CANMessageSet(CAN0_BASE, 1, &sCANMessage, MSG_OBJ_TYPE_RX);
    sCANMessage.pui8MsgData = pui8MsgData;

    for(;;){

        if(g_bRXFlag){
            /*Read the message from the CAN.*/
            CANMessageGet(CAN0_BASE, 1, &sCANMessage, 0);
            g_bRXFlag =0;

            /*If the first byte is 0x13 then we receive a new firmware*/
            if(*sCANMessage.pui8MsgData == 0x13){
                CAN_receiveFirmware();
            }
        }
    }
}


