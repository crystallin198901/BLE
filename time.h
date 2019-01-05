#ifndef TIME_H
#define TIME_H
#include"stm8s.h"
#include"ds1302.h"

//@far @interrupt void TIM4HandledInterrupt (void);

struct time_st
{
	uint16_t second;
	uint16_t msecond;
	uint16_t deltasecond;
	uint16_t deltamsecond;
        uint8_t timeout;
};

typedef struct time_st time_t; 

uint8_t time_handler(void);
void reinit_time(void);
void init_time(void);
uint8_t timeout(time_t *ptime);
uint8_t wait_ms(time_t *ptime , uint16_t delta);
uint8_t wait_second(time_t *ptime , uint16_t delta);
void update_time(time_t *ptime);
uint8_t stop_timer(time_t *ptime);
uint8_t restart_timer(time_t *ptime);/**/
#endif