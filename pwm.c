#include"pwm.h"

static uint16_t chn_duty[3];
void init_pwm(void)
{
  GPIOC->DDR |= 0xC8;
  GPIOC->CR1 |= 0xC8;
  GPIOC->CR2 |= 0xC8;
  //GPIOC->ODR |= 0xC8;
  CLK->PCKENR1 |= CLK_PCKENR1_TIM1;
  //PWM Mode 1
  TIM1->CCMR3 = TIM1->CCMR2 = TIM1->CCMR1 = 0x48;
  //0X48;
  //0X48;	
  //Output active low
  TIM1->CCER1 = TIM1_CCER1_CC1P | TIM1_CCER1_CC1E
	     | TIM1_CCER1_CC2P | TIM1_CCER1_CC2E;
  TIM1->CCER2 = TIM1_CCER2_CC3P | TIM1_CCER2_CC3E;
  //PWM Frequence
  /*
  TIM1->ARRH = 0x03;
  TIM1->ARRL = 0xE7;
  */
  TIM1->ARRH = 0x27;
  TIM1->ARRL = 0x0F;
  //PWM dutyÈ
  /*
  0;
  0;
  0;
  0;
  0;*/
  TIM1->CCR1H = TIM1->CCR1L = TIM1->CCR2H = TIM1->CCR2L = TIM1->CCR3H = TIM1->CCR3L = 0;
  //pre
  TIM1->PSCRH = 0;
  TIM1->PSCRL = 0x0;
  TIM1->BKR |= TIM1_BKR_MOE;
  TIM1->CR1 = TIM1_CR1_ARPE | TIM1_CR1_CEN;
}

void reinit_pwm(void)
{
  CLK->PCKENR1 |= CLK_PCKENR1_TIM1;
  TIM1->ARRH = 0x27;
  TIM1->ARRL = 0x0F;
  TIM1->PSCRH = 0;
  TIM1->PSCRL = 0x0;
  TIM1->BKR |= TIM1_BKR_MOE;
  TIM1->CR1 = TIM1_CR1_ARPE | TIM1_CR1_CEN;
}

uint8_t change_duty(uint16_t duty,uint8_t chn)
{
  unsigned long tmp;
  /*if(chn_duty[chn - 1] == duty)
  {
    return 1;
  }
  chn_duty[chn - 1] = duty;
  ½ÚÔ¼¿Õ¼ä*/
  if(chn == WHITE_LIGHT_CHANNAL)
  {
    tmp = (unsigned long)(10000 - 1750) * duty / 10000 + 1750;//1160
    duty = (uint16_t)tmp;
    if(duty <= 1750)
    {
      duty = 0;
    }
  }
  else if(chn == DAY_LIGHT_CHANNAL)
  {
    tmp = (unsigned long)(10000 - 1630) * duty / 10000 + 1630;//1060
    duty = (uint16_t)tmp;
    if(duty <= 1630)
    {
      duty = 0;
    }
  }
  else
  {
    tmp = (unsigned long)(10000 - 1800) * duty / 10000 + 1800;//1060
    duty = (uint16_t)tmp;
    if(duty <= 1800)
    {
      duty = 0;
    }
  }
  
  switch (chn) 
  {
  case 1 :
    if(duty == 0)
    {
      TIM1->CCMR1 &= ~0x70;
      TIM1->CCMR1 |= 0x40;
      return 1;
    }
    else if(duty >= 10000)
    {
      TIM1->CCMR1 &= ~0X70;
      TIM1->CCMR1 |= 0x50;
      return 1;
    }
    else
    {
      TIM1->CCMR1 &= ~0X70;
      TIM1->CCMR1 |= 0x60;
      TIM1->CCR1H = duty >> 8;
      TIM1->CCR1L= duty & 0xFF;
      return 1;
    }
    break;
  case 2 :
    if(duty == 0)
    {
      TIM1->CCMR2 &= ~0X70;
      TIM1->CCMR2 |= 0x40;
      return 1;
    }
    else if(duty >= 10000)
    {
      TIM1->CCMR2 &= ~0X70;
      TIM1->CCMR2 |= 0x50;
      return 1;
    }
    else
    {
      TIM1->CCMR2 &= ~0X70;
      TIM1->CCMR2 |= 0x60;
      TIM1->CCR2H = duty >> 8;
      TIM1->CCR2L = duty & 0xFF;
      return 1;
    }
    break;
  case 3 :
    if(duty == 0)
    {
      TIM1->CCMR3 &= ~0X70;
      TIM1->CCMR3 |= 0x40;
      return 1;
    }
    else if(duty >= 10000)
    {
      TIM1->CCMR3 &= ~0X70;
      TIM1->CCMR3 |= 0x50;
      return 1;
    }
    else
    {
      TIM1->CCMR3 &= ~0X70;
      TIM1->CCMR3 |= 0x60;
      TIM1->CCR3H = duty >> 8;
      TIM1->CCR3L = duty & 0xFF;
      return 1;
    }
    break;
  }
  return 0;
}