/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 */

#include"stm8s.h"
/**/
#include"time.h"
#include"pwm.h"
#include"hw.h"
#include"blecomm.h"
#include"ds1302.h"
#include"eeprom.h"
#include"def.h"
#include"plan.h"

/*
__no_init uint8_t sec_eeprom@0x405A;
__no_init uint8_t min_eeprom@0x405B;
__no_init uint8_t hour_eeprom@0x405C;
__no_init uint8_t reset_eeprom@0x4059;
__no_init uint8_t last_day_eeprom@0x405D;
__no_init uint8_t last_hour_eeprom@0x405E;
__no_init uint8_t last_min_eeprom@0x405F;
*/
extern uint8_t hour;
extern uint8_t day;

uint8_t state;
//uint8_t state = LED_NIGHT;
time_t time1;
time_t fed_dog_time;
time_t power_on_time;
static uint8_t power_on_init=0;
extern uint8_t min;
extern uint8_t sec;
extern uint8_t hour;
extern uint8_t normal_reset;
uint8_t get_power_on_state()
{
  uint8_t power_on_state;
  uint8_t power_on_sec,power_on_min;
  uint8_t state_tmp;
  uint8_t sec_tmp;
  uint8_t min_tmp;
  //uint8_t hour_tmp;
  do
  {
    sec_tmp = sec;
    min_tmp = min;
    //hour_tmp = hour;
  }while(sec!=sec_tmp);
  power_on_state = DS1302ReadOneWord(0xC1);
  power_on_sec = DS1302ReadOneWord(0xC3);
  power_on_min = DS1302ReadOneWord(0xC5);
  //power_on_hour = DS1302ReadOneWord(0xC7);
  state_tmp = LED_NIGHT;
  if(power_on_state==0)
  {
    power_on_state = 1;
  }
  else 
  {
    power_on_state = 1;
    if((power_on_min==min_tmp&&sec_tmp-power_on_sec<=7)
       ||((min_tmp < (power_on_min+2)%60)&&60+sec_tmp-power_on_sec<=7))
    {
        state_tmp = LED_NORMAL;
        power_on_state = 0;
    }
  }
  DS1302WriteOneWord(0x8E,0);
  DS1302WriteOneWord(0xC0,power_on_state);
  DS1302WriteOneWord(0xC2,sec_tmp);
  DS1302WriteOneWord(0xC4,min_tmp);
  DS1302WriteOneWord(0x8E,0x80);
  power_on_init = 1;
  //后面有wait_second函数。。。update_time(&power_on_time);
  wait_second(&power_on_time,5);
  return state_tmp;
}
int main(void)
{
  uint8_t key;
  uint16_t i;
  static uint8_t power_up;
  static uint16_t reinit_cnt = 0;
  static uint8_t normal_flag;
  uint8_t syn = 0;
  time_t syn_timer;
  uint8_t key_switch=0;
  //GPIOC->ODR |= 0xC8;
  //GPIOC->DDR |= 0xC8;
  //GPIOC->CR1 |= 0xC8;
  //GPIOC->CR2 |= 0xC8;
start:
  for(i=0;i<1200;i++)
  {
    asm("nop");
    //GPIOC->ODR |= 0xC8;
  }
  normal_flag = 0xA6;
  init_hw();
  init_time();
  init_pwm();
  init_blecomm();
  init_plan();
  asm("rim");
  //write_eeprom((uint8_t*)0x4000,&key,1);
  //DS1302WriteTimeData();
  DS1302WriteOneWord(0x90,0xA6);
  DS1302ReadTimeData();
  if(normal_reset==0) 
  {
    /*
    last_hour = hour;
    last_min = min;
    last_day = day;
    */
    state = get_power_on_state();
  }
  else
  {
    /*
    write_eeprom(&hour_eeprom,&hour,1);
    write_eeprom(&min_eeprom,&min,1);
    write_eeprom(&sec_eeprom,&day,1); 
    */
  }
    // state = LED_OFF0;
  //state = power_on_state();
  //后面有wait_second函数。。。update_time(&time1);
  wait_ms(&fed_dog_time,200);
  feed_dog();
  wait_second(&time1,7);
 // init_name();
  //switch_led(state); 
  power_up = 1;
  //switch_led(LED_MODE_NIGHT);
  //state = LED_NIGHT;
  while (normal_flag == 0xA6)
  { 
    reinit_cnt++;
    if(reinit_cnt>=0xFFF0)
    {
      reinit_uart();
      reinit_hw();
      reinit_time();
      reinit_pwm();
      reinit_cnt = 0;
    }
    if(timeout(&fed_dog_time)!=0) feed_dog();
    blecomm_handler();
    if(timeout(&fed_dog_time)!=0) 
    {
      wait_ms(&fed_dog_time,200);
      feed_dog();
    }
    key = scan_key();
    time_handler();
    plan_handler();
    if(power_on_init)
    {
      if(timeout(&power_on_time))
      {
        power_on_init = 0;
        DS1302WriteOneWord(0xC0,0);
        /*
        write_eeprom(&last_day_eeprom,&last_day,1);
        write_eeprom(&last_hour_eeprom,&last_hour,1);
        write_eeprom(&last_min_eeprom,&last_min,1);
        */
      }
    }
    
    
    if(key_switch&&(state == LED_OFF2))
    {
      if(timeout(&time1))
      {
        key_switch = 0;
        state = LED_OFF0;
      }
    }
    /*
    if(timeout(&time1)==1)
    {
      restart_timer(&time1);
      if(state == LED_OFF2)
      {
        state = LED_OFF0;
      }
      else if(state == LED_NIGHT)
      {
        state = LED_NORMAL;
      }
    }
    */
    
    if(key == TOUCH_KEY || key == SWITCH_KEY || power_up)
    //if(key == TOUCH_KEY || power_up)
    {
      power_up = 0;
       
      if(key == SWITCH_KEY)
      {
        uint8_t sec_tmp,min_tmp;
        do
        {
          sec_tmp = sec;
          min_tmp = min;
        }
        while(sec_tmp != sec);
        DS1302WriteOneWord(0x8E,0);
        if(state == LED_NIGHT  || state == LED_OFF2)
        {
          state = LED_NORMAL;
          DS1302WriteOneWord(0xC0,1);
        }
        else
        {
          state = LED_NIGHT;
          DS1302WriteOneWord(0xC0,0);
        }
        DS1302WriteOneWord(0xC2,sec_tmp);
        DS1302WriteOneWord(0xC4,min_tmp);
        DS1302WriteOneWord(0x8E,0x80);
        syn = 1;
        wait_second(&syn_timer,7);
      }
      else if(key == TOUCH_KEY)
      {
        state++;
        if(state == LED_OFF2)
        {
          key_switch = 1;
          restart_timer(&time1);
        }
      }
      if(syn)
      {
         //if((GPIOB->IDR & 0x20) == 0)
         if(timeout(&syn_timer))
         {
           //syn++;
           //if(syn >= 200)
           {
             DS1302WriteOneWord(0x8E,0);
             /*
             if(state == LED_NORMAL)
             { 
               DS1302WriteOneWord(0xC0,1);
             }
             else if(state == LED_NIGHT)
             {
               DS1302WriteOneWord(0xC0,0);
             }
             */
             DS1302WriteOneWord(0xC0,0);
             DS1302WriteOneWord(0x8E,0x80);
             syn = 0;
           }
         }
      }
      
      
      state %= (LED_NORMAL+1);
      switch (state)
      {
        case LED_OFF0 :
        case LED_OFF2 :
          switch_led(LED_MODE_OFF);            
          break;
        case LED_NIGHT :
          switch_led(LED_MODE_NIGHT); 
          break;
        case LED_NORMAL :
          switch_led(LED_MODE_NORMAL); 
          break;
        }
    } 
    /*
    else if(key == POWER_OFF)
    {
      turn_off_led();
    }
    */
  }
  goto start;
  return 1;
}