#ifndef PLAN_H
#define PLAN_H
#include"stm8s.h"
#include"ds1302.h"
#include"pwm.h"
#include"time.h"
#include"eeprom.h"
#define PLAN_MAX_NUM 10
#define LED_MODE_OFF 1
#define LED_MODE_NIGHT 2
#define LED_MODE_NORMAL 3
#define LED_MODE_OFF_BEGIN 4
#define LED_MODE_LAST 6
#define LED_OFF 5
#define MODE_ALARM 0
#define MODE_NIGHT 1
#define MODE_COLOR_TEMP 2
#define MODE_LIGHT_COLOR 3
#define CLOSE 0
#define RUNNING 1
#define HALT 2
void init_plan(void);
void plan_handler(void);
void read_plan(void);
void switch_led(uint8_t); 
void turn_off_led(void);
void halt_alarm(void);
void halt_plan(uint8_t);
struct light_color_st
{
	uint8_t white;//6500K
	uint8_t day;//4000K
	uint8_t warm;//2700K
};

struct light_temp_st
{
  uint16_t color_temp;
  uint8_t brightness;
};

struct demo_st
{
  uint8_t mode;
  union
  {
    uint16_t color_temp;
    struct light_color_st light_color;
  }light;
  uint8_t brightness;
};

struct plan_st
{
  uint8_t open_hour;
  uint8_t open_min;
  uint8_t close_hour;
  uint8_t close_min;
  //uint8_t mode;
  uint16_t color_temp;
  uint8_t brightness;
};
typedef struct plan_st plan_t;

struct alarm_st
{
  uint8_t hour;
  uint8_t min;
  uint8_t duration;
  uint8_t halt;
};
typedef struct alarm_st alarm_t;

struct led_st
{
  uint8_t mode;
  struct 
  {
    struct light_temp_st light_temp;
    struct light_color_st light_color;
  }light;
};
typedef struct led_st led_t;
#endif