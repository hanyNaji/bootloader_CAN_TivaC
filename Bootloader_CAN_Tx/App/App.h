/*
 * App.h
 *
 *  Created on: Sep 29, 2023
 *      Authors:
 *          - Hany Nagy - Muhammed Al shafie - Waleed Gamal - Hamdy Elsayd
 */

#ifndef APP_APP_H_
#define APP_APP_H_


/* Define the CAN message structure */
typedef struct {
    uint32_t ui32MsgID;
    uint32_t ui32MsgIDMask;
    uint32_t ui32Flags;
    uint32_t ui32MsgLen;
    uint8_t* pui8MsgData;
} CANMsgObject;

/* Function prototypes */

void system_Config(void);
void Can_Init(void);
void CAN_MessageInit(tCANMsgObject* msgObject, uint32_t msgID, uint32_t msgIDMask, uint32_t tx_rx, uint32_t msgLen, uint8_t* ptrData);
void SimpleDelay(void);
void CAN_sendData(uint32_t canBase, uint32_t msgObjectNum, tCANMsgObject* msg);
void CAN_receiveData(uint32_t canBase, uint32_t msgObjectNum, tCANMsgObject* msg);
void CANIntHandler(void);


#endif /* APP_APP_H_ */
