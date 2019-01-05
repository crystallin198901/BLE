#ifndef HW_H
#define HW_H
#include"stm8s.h"
#include"time.h"

#define TOUCH_KEY 0x01
#define SWITCH_KEY 0x02
#define POWER_OFF 0x04
void init_hw(void);
uint8_t scan_key(void);
void feed_dog(void);
void init_hw(void);
void reinit_hw(void);
#endif