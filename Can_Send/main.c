
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"

#include "App/App.h"
#include "App/Flash1_image_0.h"
#include "App/Flash2_image_0.h"



#define FIRMWARE_LEN    9024


volatile uint8_t sendFirmware_flag = 0;
extern volatile uint32_t g_ui32MsgCount ;

void GPIOFIntHandler(void)
{
    /* Clear the interrupt for GPIO_PIN_4 */
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
    /* Set flag to send the firmware */
    sendFirmware_flag = 1;
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
            /* Reset the message count */
            g_ui32MsgCount = 0;

            /* Clear the sendFirmware_flag */
            sendFirmware_flag = 0;

            /* Set GPIO_PORTF_BASE, GPIO_PIN_1 high */
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);

            /* Set the CAN message data pointer to pui8MsgData1 */
            sCANMessage.pui8MsgData = pui8MsgData1;

            /* Send a CAN message with object 1 */
            CANMessageSet(CAN0_BASE, 1, &sCANMessage, MSG_OBJ_TYPE_TX);

            uint16_t size;

            /* Loop through firmware data blocks */
            for (size = 0; size < FIRMWARE_LEN / 8; size++)
            {
                int i;

                /* Delay loop for a brief period */
                for (i = 0; i < 500; i++);

                /* Set the CAN message data pointer to the current firmware data block */
                sCANMessage.pui8MsgData = FLash1_image_0 + (size * 8);

                /* Send a CAN message with object 1 */
                CANMessageSet(CAN0_BASE, 1, &sCANMessage, MSG_OBJ_TYPE_TX);
            }

            /* Call a simple delay function */
            SimpleDelay();

            /* Clear GPIO_PORTF_BASE, GPIO_PIN_1 */
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
        }
    }

}
