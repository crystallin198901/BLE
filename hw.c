#include"hw.h"
time_t touch_hold_time;
time_t switch_hold_time;
extern uint8_t BLE_reset_flag;

void reinit_hw(void)
{
  GPIOD->DDR |= 0x04;
  GPIOD->CR1 |= 0x04;
  if(BLE_reset_flag) 
    GPIOA->ODR |= 0x08;
  GPIOA->CR1 |= 0x08;
  if(BLE_reset_flag) 
    GPIOA->ODR |= 0x08;
  GPIOA->CR1 |= 0x08;
  //set as input touchkey input PIN
  GPIOC->DDR &= ~0x10;
  
  //set as input switch input PIN 
  GPIOB->DDR &= ~0x20;
}
void init_hw(void)
{
  CLK->PCKENR1 = 0x00;
  CFG->GCR = 00;
  //Enable EXT OSC
  CLK->ECKR = 0x01;
  CLK->ICKR = 0x01;
  //Selset EXT OSC
  CLK->CMSR = 0xB4;
  //INT OSC 8mhz
  CLK->CKDIVR = 0x08;
  //while(!(CLK->ICKR & CLK_ICKR_HSIRDY));
  while(!(CLK->ECKR & CLK_ECKR_HSERDY));
  CLK->SWCR = CLK_SWCR_SWEN;
  CLK->SWR = 0xB4;
  
  while(CLK->SWCR&0x01);
  CLK->SWCR = 0;
  //set as output power light PIN 
  GPIOD->DDR |= 0x04;
  GPIOD->CR1 |= 0x04;
  GPIOD->ODR |= 0x04;
  
  //set as ouput BLERST
  GPIOA->ODR |= 0x08;
  GPIOA->CR1 |= 0x08;
  GPIOA->DDR |= 0x08;
  
  //set as input touchkey input PIN
  GPIOC->DDR &= ~0x10;
  
  //set as input switch input PIN 
  GPIOB->DDR &= ~0x20;
  //GPIOC->CR1 |= 0X10;
  //后面有wait_second函数。。。update_time(&touch_hold_time);
  wait_ms(&touch_hold_time,250);
  //后面有wait_second函数。。。update_time(&switch_hold_time);
  wait_ms(&switch_hold_time,10);
  
  /*enable watchdog*/
  //watchdog enable
  IWDG->KR = 0xCC;
  //Access enable
  IWDG->KR = 0x55;
  //256 div
  IWDG->PR = 0x06;
  //Access enable
  IWDG->KR = 0x55;
  //reload value
  IWDG->RLR = 0xFF; 
  IWDG->KR = 0xAA;
  
  //output system clock
  CLK->CCOR = 0x13;
}

void feed_dog(void)
{/**/
  IWDG->KR = 0xAA;
}

uint8_t scan_key(void)
{
  static uint8_t key = 0;

	if((GPIOC->IDR & 0x10) == 0)
	{
		//update_time(&touch_hold_time);
		wait_ms(&touch_hold_time,150);
                if(key & TOUCH_KEY)
                {
                  key &= ~TOUCH_KEY;
                  return TOUCH_KEY;
                }
	}
	else
	{
		if(timeout(&touch_hold_time) == 1)
		{
			key |= TOUCH_KEY;
		}
	}
	/**/
	if((GPIOB->IDR & 0x20) == 0)
	{
		//update_time(&switch_hold_time);
		wait_ms(&switch_hold_time,5);
                /*
                if(key & SWITCH_KEY)
                {
                  key &= ~SWITCH_KEY;
                  return SWITCH_KEY;
                }
                */
                return 0;
	}
	else
	{
		if(timeout(&switch_hold_time) == 1)
		{
                  //update_time(&switch_hold_time);
		wait_second(&switch_hold_time,2);
                if((key & SWITCH_KEY)==0)
                {
                  return SWITCH_KEY;
                }
                else
                {
			key |= SWITCH_KEY;
                        return POWER_OFF;
                }
		}
	}
        
	return 0;
}