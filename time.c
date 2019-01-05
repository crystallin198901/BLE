#include"time.h"
//static uint8_t calc=0;
static uint16_t ms = 980;
static uint16_t s = 0xFFFE;
extern uint8_t sec;
extern uint8_t min;
extern uint8_t hour;
extern uint8_t year;


//TIM4 ÖÐ¶Ï³öÀ´º¯Êý£¬Ã¿1ms²úÉúÒ»´ÎÖÐ¶Ï
#pragma vector=0x19 
__interrupt void TLI_IRQHandler(void)
{
  	TIM4->SR1 = 0x00;
  ms++;
	if(ms == 1000)
	{
		ms = 0;
		s++;
		sec++;
		//GPIOC->ODR ^= 0x10;
	}
}

uint8_t time_handler(void)
{
	if(sec >= 60)
	{
		sec = 0;
		min++;
		if(min >= 60)
		{
			min = 0;
			hour++;
			if(hour >= 24)
			{
				hour = 0;
			}
		}
	}
	if(sec == 30)
	{
	  DS1302ReadTimeData();
	}
       return 1;
}
//TIM4 ³õÊ¼»¯º¯Êý
void init_time(void)
{
	CLK->PCKENR1 |= CLK_PCKENR1_TIM4;
	TIM4->CR1 = TIM4_CR1_ARPE;
	//TIM4->ARR = 249;//8Mhz
        TIM4->ARR = 229;
	//8·ÖÆµ
	//TIM4->PSCR = 0x03;
        //32·ÖÆµ
        TIM4->PSCR = 0x05;
        //64
        //TIM4->PSCR = 0x06;
	TIM4->IER = TIM4_IER_UIE;
	TIM4->EGR = 0x00;
	TIM4->SR1 = 0x00;
	TIM4->CNTR = 0x00;
	//TIM4->CR1 = TIM4_CR1_ARPE | TIM4_CR1_URS | TIM4_CR1_CEN;
	TIM4->CR1 |= TIM4_CR1_CEN;
}

void reinit_time(void)
{
  CLK->PCKENR1 |= CLK_PCKENR1_TIM4;
  TIM4->CR1 = TIM4_CR1_ARPE;
  //TIM4->ARR = 249;//8Mhz
  TIM4->ARR = 229;
  TIM4->PSCR = 0x05;
  TIM4->IER = TIM4_IER_UIE;
  TIM4->EGR = 0x00;
  TIM4->SR1 = 0x00;
  TIM4->CR1 |= TIM4_CR1_CEN;
}
uint8_t timeout(time_t* ptime)
{
  uint16_t tmp=0,s_tmp,ms_tmp;
  uint8_t ret_value = 0xFF;
  if(ptime->timeout > 0) return 1;
  do
  {
    ms_tmp = ms;
    s_tmp = s;
  }while(ms_tmp != ms);
  if(ptime->deltasecond > 0)
  {
    ret_value = 0;
    if(s_tmp >= ptime->second)
    {
      if(s_tmp - ptime->second > ptime->deltasecond)
      {
        ret_value = 1;
      }
      else if(s_tmp - ptime->deltasecond == ptime->second)
      {
        if(ms_tmp >= ptime->msecond)
        {
          ret_value = 1;
        }
      } 
    }
    else
    {
      tmp = 0xFFFF - ptime->second + s_tmp + 1;
      if(tmp > ptime->deltasecond)
      {
        ret_value = 1;
      }
      else if(tmp == ptime->deltasecond)
      {
        if(ms_tmp >= ptime->msecond)
        {
          ret_value = 1;
        }
      }
    }
  }
  else if(ptime->deltamsecond > 0)
  {
    ret_value = 0;
    if(s_tmp >= ptime->second)
    {
      tmp = s_tmp - ptime->second;
      //³¬¹ýÁË65Ãë£¬ÒÑ¾­³¬³ö¼ÆÊ±Çø¼äý
      if(tmp > 65)
      {
        ret_value = 1;
      }
      tmp = 1000 * tmp + ms_tmp - ptime->msecond;
      if(tmp >= ptime->deltamsecond)
      {
        ret_value = 1;
      }
    }
    else if(s_tmp < ptime->second)
    {
      tmp = 0xFFFF - ptime->second + s_tmp + 1;
      if(tmp > 65)
      {
        ret_value = 1;
      }
      tmp = 1000 * tmp + ms_tmp - ptime->msecond;
      if(tmp >= ptime->deltamsecond)
      {
        ret_value = 1;
      }
    }
  }
  if(ret_value==1) ptime->timeout = 1;
  return ret_value;
}

uint8_t wait_ms(time_t* ptime , uint16_t delta)
{
  ptime->timeout = 0;
  ptime->deltasecond = 0;
  do
  {
    ptime->second = s;
    ptime->msecond = ms;
  }while(ptime->second != s);
  ptime->deltamsecond = delta;
  return 1;
}

uint8_t wait_second(time_t* ptime , uint16_t delta)
{
  ptime->timeout = 0;
  ptime->deltamsecond = 0;
  do
  {
    ptime->second = s;
    ptime->msecond = ms;
  }while(ptime->second != s);
  ptime->deltasecond = delta;
  return 1;
} 

uint8_t stop_timer(time_t* ptime)
{
  ptime->timeout = 0;
  ptime->deltasecond = 0;
  ptime->deltamsecond = 0;
  return 1;
}

uint8_t restart_timer(time_t* ptime)
{
  ptime->timeout = 0;
	if(ptime->deltasecond == 0 && ptime->deltamsecond == 0)
	{
		return 0;
	}
	else
	{
		do
		{
			ptime->second = s;
			ptime->msecond = ms;
		}while(ptime->second != s);
		return 1;
	}
}

/*
void update_time(time_t *ptime)
{
  ptime->timeout = 0;
  do
  {
    ptime->second = s;
    ptime->msecond = ms;
  }while(ptime->second != s);
}
*/