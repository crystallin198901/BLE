#ifndef PWM_H
#define PWM_H
#include"stm8s.h"

#define WHITE_LIGHT_CHANNAL 2
#define DAY_LIGHT_CHANNAL 1
#define WARM_LIGHT_CHANNAL 3

void init_pwm(void);
void reinit_pwm(void);
uint8_t change_duty(uint16_t duty,uint8_t chn);
#endif