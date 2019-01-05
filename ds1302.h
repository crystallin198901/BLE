#ifndef DS1302_H
#define DS1302_H
#include"stm8s.h"
/*
#define InitDS1302ClockPort GPIOB->DDR |= 0x20;GPIOB->CR1 |= 0x20;GPIOB->CR2 |= 0x20
#define	InitDS1302ChipEnablePort GPIOC->DDR |= 0x20;GPIOC->CR1 |= 0x20;GPIOC->CR2 |= 0x20
#define SetDS1302DataPortWriteMode GPIOB->DDR |= 0x10;GPIOB->CR1 |= 0x10;GPIOB->CR2 |= 0x10
#define SetDS1302DataPortReadMode GPIOB->DDR &= ~0x10;GPIOB->CR1 |= 0x10;GPIOB->CR2 &= ~0x10
#define DS1302Rst_0 GPIOC->ODR &= ~0x20
#define DS1302Rst_1 GPIOC->ODR |= 0x20
#define DS1302Sclk_0 GPIOB->ODR &= ~0x20
#define DS1302Sclk_1 GPIOB->ODR |= 0x20
#define DS1302Data_0 GPIOB->ODR &= ~0x10
#define DS1302Data_1 GPIOB->ODR |= 0x10
#define DS1302Data  GPIOB->IDR & 0x10
*/

#define InitDS1302ClockPort GPIOD->CR1 |= 0x10;GPIOD->CR2 |= 0x10;GPIOD->DDR |= 0x10
#define	InitDS1302ChipEnablePort GPIOD->CR1 |= 0x08;GPIOD->CR2 |= 0x08;GPIOD->DDR |= 0x08
#define SetDS1302DataPortWriteMode GPIOC->CR1 |= 0x20;GPIOC->DDR |= 0x20
#define SetDS1302DataPortReadMode GPIOC->CR1 |= 0x20;GPIOC->CR2 &= ~0x20;GPIOC->DDR &= ~0x20
#define DS1302Rst_0 GPIOD->ODR &= ~0x08
#define DS1302Rst_1 GPIOD->ODR |= 0x08
#define DS1302Sclk_0 GPIOD->ODR &= ~0x10
#define DS1302Sclk_1 GPIOD->ODR |= 0x10
#define DS1302Data_0 GPIOC->ODR &= ~0x20
#define DS1302Data_1 GPIOC->ODR |= 0x20
#define DS1302Data  GPIOC->IDR & 0x20



void DS1302ReadTimeData(void);
void DS1302WriteTimeData(void);
void DS1302WriteOneWord(uint8_t,uint8_t);
uint8_t DS1302ReadOneWord(uint8_t);
#endif