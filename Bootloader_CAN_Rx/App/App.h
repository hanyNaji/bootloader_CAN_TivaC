/*
 * App.h
 *
 *  Created on: Sep 29, 2023
 *      Authors:
 *          - Hany Nagy - Muhammed Al shafie - Waleed Gamal - Hamdy Elsayd
 */

#ifndef APP_APP_H_
#define APP_APP_H_

/* Include TivaWare library header files */
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/can.h"
#include "inc/hw_can.h"


/* System Configurations */
void system_Config(void);

/* CAN Functions prototypes */


void Can_Init(void);
void CAN_MessageInit(tCANMsgObject* msgObject, uint32_t msgID, uint32_t msgIDMask, uint32_t tx_rx, uint32_t msgLen, uint8_t* ptrData);
void SimpleDelay(void);

void CANIntHandler(void);
void CAN_receiveFirmware(void);

void Blue_LED_Toggle(void);
void Green_LED_Toggle(void);

/* BootLoader Functions prototypes */

void JumpToBootLoader(uint32_t addr);

void UpdateFirmWare(uint32_t* firmwareData, uint32_t addr, uint16_t firmwareData_Size);



#endif /* APP_APP_H_ */
