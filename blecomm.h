#ifndef BLECOM_H
#define BLECOM_H
#include"stm8s.h"
#include"time.h"
#include"plan.h"
#include"ds1302.h"
#include"eeprom.h"
#include"def.h"
#define TX_BUF_LEN 100
#define RX_BUF_LEN 100
void init_blecomm(void);
void reinit_uart(void);
void blecomm_handler(void);
void blecomm_send(uint8_t,uint8_t);
void init_name(void);
//@far @interrupt void UART1TxHandlerInterrupt (void);
//@far @interrupt void UART1RxHandlerInterrupt (void);
#endif