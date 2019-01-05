#ifndef EEPROM_H
#define EEPROM_H
#include"stm8s.h"
uint8_t write_eeprom(uint8_t *addr,uint8_t *pdata,uint8_t len);
#endif