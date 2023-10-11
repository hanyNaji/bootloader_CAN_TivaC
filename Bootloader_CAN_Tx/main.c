/*
 * main.c
 *
 *  Created on: Oct 10, 2023
 *      Author: hany Nagy
 */

/*Include standard C libraries*/
#include <stdbool.h>
#include <stdint.h>

/* Include a custom header file */
#include "App/App.h"
#include "App/Flash1_image_0.h"
#include "App/Flash2_image_0.h"


/* New Firmware length */
#define FIRMWARE_1_LEN    5056
#define FIRMWARE_2_LEN    5048


volatile uint8_t sendFirmware_flag = 0;
extern volatile uint32_t g_ui32MsgCount ;


#define GPIO_PORTF_MIS_R    (*((volatile unsigned long *)0x40025418))

void GPIOFIntHandler(void)
{
    /* check if interrupt causes by PF4/SW1*/
    if (GPIO_PORTF_MIS_R & GPIO_PIN_4)
    {
        /* Clear the interrupt for GPIO_PIN_4 */
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
        /* Set flag to send the firmware */
        sendFirmware_flag = 1;
    }
    /* check if interrupt causes by PF0/SW2 */
    else if (GPIO_PORTF_MIS_R & GPIO_PIN_0)
    {
        /* Clear the interrupt for GPIO_PIN_1 */
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
        /* Set flag to send the firmware */
        sendFirmware_flag = 2;
    }
    else {
        /* Do nothing for MISRA */
    }
}

void main (){
    /* Initialize System Configurations */
    system_Config();


    tCANMsgObject sCANMessage;
    uint8_t pui8MsgData1[8] = {0x13};
    /*
     * Initialize a message object to be used for receiving CAN messages with
     * any CAN ID.  In order to receive any CAN ID, the ID and mask must both
     * be set to 0, and the ID filter enabled.
     */
    sCANMessage.ui32MsgID = 1;
    sCANMessage.ui32MsgIDMask = 0;
    sCANMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    sCANMessage.ui32MsgLen = 8;


    for (;;) {

        /* Check if the sendFirmware_flag is set */
        if (sendFirmware_flag)
        {
            /* Clear GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 */
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x0);

            /* Reset the message count */
            g_ui32MsgCount = 0;

            /* Set the CAN message data pointer to pui8MsgData1 */
            sCANMessage.pui8MsgData = pui8MsgData1;

            /* Send a CAN message with object 1 */
            CANMessageSet(CAN0_BASE, 1, &sCANMessage, MSG_OBJ_TYPE_TX);

            uint16_t size;
            if(sendFirmware_flag==1)
            {
                /* Set GPIO_PORTF_BASE, GPIO_PIN_2 high */
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);

                /* Loop through firmware data blocks */
                for (size = 0; size < FIRMWARE_1_LEN / 8; size++)
                {
                    int i;

                    /* Delay loop for a brief period */
                    for (i = 0; i < 500; i++);

                    /* Set the CAN message data pointer to the current firmware data block */
                    sCANMessage.pui8MsgData = Flash1_image_0 + (size * 8);

                    /* Send a CAN message with object 1 */
                    CANMessageSet(CAN0_BASE, 1, &sCANMessage, MSG_OBJ_TYPE_TX);
                }
            }
            else if(sendFirmware_flag==2)
            {
                /* Set GPIO_PORTF_BASE, GPIO_PIN_1 high */
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);

                /* Loop through firmware data blocks */
                for (size = 0; size < FIRMWARE_2_LEN / 8; size++)
                {
                    int i;

                    /* Delay loop for a brief period */
                    for (i = 0; i < 500; i++);

                    /* Set the CAN message data pointer to the current firmware data block */
                    sCANMessage.pui8MsgData = Flash2_image_0 + (size * 8);

                    /* Send a CAN message with object 1 */
                    CANMessageSet(CAN0_BASE, 1, &sCANMessage, MSG_OBJ_TYPE_TX);
                }

            }
            else{
                /*  */
            }


            /* Clear the sendFirmware_flag */
            sendFirmware_flag = 0;

            /* Call a simple delay function */
            SimpleDelay();

            /* Clear GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 */
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x0);
            /* Set GPIO_PORTF_BASE, GPIO_PIN_1 high */
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
        }

    }

}
