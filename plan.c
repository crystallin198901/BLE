#include"plan.h"
#include"def.h"

void blecomm_send(uint8_t,uint8_t);
extern uint8_t hour;
extern uint8_t min;
extern uint8_t sec;
extern uint8_t state;
extern uint8_t ble_connection;

static uint16_t warm_tmp = 0,day_tmp = 0,white_tmp = 0;
static time_t smooth_time;
static time_t alarm_time;
uint16_t warm_duty = 0,day_duty = 0,white_duty = 0;
static uint8_t state_mode = LED_MODE_NIGHT;
//static uint8_t last_brightness = 50;
static uint8_t finish_change = 0;
__no_init led_t last;
static volatile uint8_t tmp;
static uint8_t alarm_running = 0;
static uint8_t first_reset = 1;
uint8_t plan_halt = 1;

led_t night;
led_t plan_before;
led_t alarm_before;
led_t rightnow;
uint8_t plan_record=0;
uint8_t alarm_state;
uint8_t plan_state;

alarm_t alarm;
uint8_t update=0;
uint8_t fixity_brightness=0;
uint16_t inc_value,dec_value;
uint8_t uc_warm,uc_day,uc_white;
const led_t OFF_led = {MODE_LIGHT_COLOR,0,0,0,0,0};

plan_t plan[PLAN_MAX_NUM];
//led_t led[PLAN_MAX_NUM];
//uint8_t plan_halt[PLAN_MAX_NUM]={0,0,0,0,0};
uint8_t opened[PLAN_MAX_NUM]={0};
time_t send_plan_time;
uint8_t send_plan=0;
uint8_t send_plan_num=0;

__no_init led_t night_eeprom@0x4070;
__no_init plan_t plan_eeprom[10]@0x4000;
//__no_init led_t led_eeprom[5]@0x4030;
__no_init alarm_t alarm_eeprom@0x4060;
__no_init uint8_t plan_halt_eeprom@0x406A;
__no_init uint8_t init_flag_eeprom@0x407A;
/**/
uint16_t light_color_calc_duty(uint16_t light_color, uint16_t brightness)
{
  uint16_t ret;
  unsigned long f;
  f = (unsigned long)light_color * brightness;
  ret = (uint16_t)f;
  ret = ret > 10000 ? 10000 : ret;
  return ret;
}

void calc_duty(uint16_t color_temp, uint16_t brightness)
{
  unsigned long a;
  if(color_temp < 2200 || color_temp > 6500) return;
  if(color_temp < 4000)
  {
    a = 10000 * (unsigned long)(color_temp - 2200) / (4000 - 2200);
    warm_duty = (uint16_t)a;
    a = a * brightness / 100; 
    day_duty = (uint16_t)a;
    warm_duty = 10000 - warm_duty;
    a = (unsigned long)brightness * warm_duty / 100; 
    warm_duty = (uint16_t)a;
    white_duty = 0;
  }
  else
  {
    a = 10000 * (unsigned long)(6500 - color_temp) / (6500 - 4000);
    white_duty = (uint16_t)a;
    a = a * brightness / 100; 
    day_duty = (uint16_t)a;
    white_duty = 10000 - white_duty;
    a = (unsigned long)brightness * white_duty / 100; 
    white_duty = (uint16_t)a;
    warm_duty = 0;
  }
}

void update_duty(led_t led)
{
  if(led.mode == MODE_COLOR_TEMP)
  {
    calc_duty(led.light.light_temp.color_temp, led.light.light_temp.brightness);
  }
  else if(led.mode == MODE_LIGHT_COLOR)
  {
    white_duty = light_color_calc_duty(led.light.light_color.white,
                                       70);
    day_duty = light_color_calc_duty(led.light.light_color.day,
                                       70);
    warm_duty = light_color_calc_duty(led.light.light_color.warm,
                                       70);
  }
  rightnow = led;
  uc_warm = warm_duty/100;
  uc_day = day_duty/100;
  uc_white = white_duty/100;
}
void read_plan(void)
{
  uint8_t i;
  for(i=0;i<PLAN_MAX_NUM;i++)
  {
    plan[i] = plan_eeprom[i];
    //led[i] = led_eeprom[i];
  }
}


void init_plan(void)
{
  uint8_t i;
  /*
  for(i=0;i<PLAN_MAX_NUM;i++)
  {
    plan[i].open_hour = 15;
    plan[i].open_min = 10+2*i;
    plan[i].close_hour = 15;
    plan[i].close_min = 11+2*i;
    //plan[i].mode = i;
    plan[i].color_temp = 6500 - 300 * i;
    plan[i].brightness = 100;
  }*/
  //plan[1].open_min = 12;
  /*
  led[1].light.light_color.day = 50;
  led[1].light.light_color.warm = 50;
  led[1].light.light_color.white = 50;
  led[1].mode = MODE_LIGHT_COLOR;
  */
    /*
  night.light.light_temp.brightness = 30;
  night.mode = MODE_COLOR_TEMP;
  night.light.light_temp.color_temp = 2200;
  alarm.hour = 9;
  alarm.min = 0;
  alarm.duration = 18;
  
  write_eeprom((uint8_t*)plan_eeprom,(uint8_t*)plan,sizeof(plan));
  //write_eeprom((uint8_t*)led_eeprom,(uint8_t*)led,sizeof(led));
  write_eeprom((uint8_t*)&night_eeprom,(uint8_t *)&night,sizeof(night));
  write_eeprom((uint8_t*)&alarm_eeprom,(uint8_t*)&alarm,sizeof(alarm));
  */
  //后面有wait_second函数。。。update_time(&smooth_time);
  wait_ms(&smooth_time,10);
  //write_eeprom((uint8_t*)plan_eeprom,(uint8_t *)plan,(1+PLAN_MAX_NUM)*sizeof(plan_t));
  state_mode = LED_MODE_NIGHT;

  read_plan();
  night = night_eeprom;
  alarm = alarm_eeprom;
  plan_halt = plan_halt_eeprom;

  if(init_flag_eeprom!=0x55)
  {
    /*
    for(i=0;i<PLAN_MAX_NUM;i++)
    {
      plan[i].open_hour = i;
      //plan[i].open_min = 0;
      plan[i].close_hour = i;
      //plan[i].close_min = 0;
      plan[i].color_temp = 2200;
      plan[i].brightness = 30;
    } */
    alarm.duration = 15;
    alarm.halt = 0;
    //alarm.hour = 6;
   //alarm.min = 0;
    plan_halt = 0;
    night.mode = MODE_COLOR_TEMP;
    night.light.light_temp.color_temp = 2200;
    night.light.light_temp.brightness = 30;
    i=0x55;
   // write_eeprom((uint8_t*)plan_eeprom,(uint8_t *)plan,(PLAN_MAX_NUM)*sizeof(plan_t));
    write_eeprom((uint8_t*)&alarm_eeprom,(uint8_t*)&alarm,sizeof(alarm_t));
    write_eeprom((uint8_t*)&plan_halt_eeprom,(uint8_t*)&plan_halt,sizeof(uint8_t));
    write_eeprom((uint8_t*)&night_eeprom,(uint8_t*)&night,sizeof(night));
    write_eeprom((uint8_t*)&init_flag_eeprom,(uint8_t*)&i,sizeof(i));
  }
  //last = night;
  //update_duty(night);
}
uint16_t adjust_duty(uint16_t tmp_duty,uint16_t duty)
{
  uint16_t delta_value;
  uint16_t old_duty;
  old_duty = tmp_duty;
  if(tmp_duty > duty)
    {
      delta_value = 10 + ((tmp_duty - duty) >> 8);
      if(tmp_duty<1000)
      {
        delta_value >>= 1;
      }
      if(fixity_brightness)
      {
        if(inc_value)
        {
          delta_value = inc_value;
          inc_value = 0;
          finish_change = 1;
        }
        else if(dec_value)
        {
          delta_value=0;
        }
        else
        {
          dec_value = delta_value;
        }
      }
      tmp_duty = tmp_duty >= delta_value ? tmp_duty - delta_value : 0;
      if(tmp_duty < duty)
      {
        tmp_duty = duty;          
      }
      if(fixity_brightness>0 && dec_value > 0)
      {
        dec_value = old_duty - tmp_duty;
      }
    }
    else if (tmp_duty < duty)
    {
      delta_value = 10 + ((duty - tmp_duty) >> 8);
      if(tmp_duty<1000)
      {
        delta_value >>= 1;
      }
      if(warm_tmp<=1000&&white_tmp<=1000&&day_tmp<=1000)
      {
        delta_value = 5;
      }
      if(fixity_brightness)
      {
        if(dec_value)
        {
          delta_value = dec_value;
          dec_value = 0;
          finish_change = 1;
        }
        else if(inc_value)
        {
          delta_value = 0;
        }
        else
        {
          inc_value = delta_value;
        }
      }
      tmp_duty += delta_value;
      if(tmp_duty > duty)
      {
        tmp_duty = duty;
      }
      if(fixity_brightness>0 && inc_value > 0)
      {
        inc_value = tmp_duty - old_duty;
      }
    }
  return tmp_duty;
}
/*
uint16_t adjust_duty(uint16_t tmp_duty,uint16_t duty)
{
  uint16_t delta_value;
  if(tmp_duty > duty)
    {
      delta_value = 20 + ((tmp_duty - duty) >> 11);
      if(fixity_brightness)
      {
        if(fixity_delta_value)
        {
          delta_value = fixity_delta_value;
          finish_change = 1;
        }
        else
        {
          fixity_delta_value = delta_value;
        }
      }
      tmp_duty = tmp_duty >= delta_value ? tmp_duty - delta_value : 0;
      if(tmp_duty < duty)
      {
        tmp_duty = duty;          
      }
    }
    else if (tmp_duty < duty)
    {
      delta_value = 20 + ((duty - tmp_duty) >> 11);
      if(warm_tmp<=1000&&white_tmp<=1000&&day_tmp<=1000)
      {
        delta_value = 5;
      }
      if(fixity_brightness)
      {
        if(fixity_delta_value)
        {
          delta_value = fixity_delta_value;
          finish_change = 1;
        }
        else
        {
          fixity_delta_value = delta_value;
        }
      }
      tmp_duty += delta_value;
      if(tmp_duty > duty)
      {
        tmp_duty = duty;
      }
    }
  return tmp_duty;
}*/

void halt_alarm(void)
{
  if(alarm_running)
  {
    update_duty(alarm_before);
    alarm_running = 0;
  }
}
void halt_plan(uint8_t flag)
{
  uint8_t i;
  if(plan_halt==1)
  {
    for(i=0;i<PLAN_MAX_NUM;i++)
    {
      if(opened[i]==RUNNING)
      {
        opened[i]=HALT;
        if(flag==1)
        {
          if(plan_record)
          {
            update_duty(plan_before);
            plan_record = 0;
          }
        }
        else
        {
          plan_record = 0;
        }
        if(plan_state == LED_OFF0 || plan_state == LED_OFF2)
          state = LED_OFF0;
        else
          state = LED_NORMAL;
        break;
      }
    }
    
  }
}
void plan_handler(void)
{
  //static uint8_t current_mode = 0;
  static uint8_t reset = 0;
  static time_t reset_timer;
  led_t plan_led;
  uint8_t i = 0,j = 0;
  finish_change = 0;
  //fixity_delta_value = 0;
  inc_value = 0;
  dec_value = 0;
  if(update == 1)
  {
    alarm_running = 0;
    update = 0;
    update_duty(last);
  }
  
  if(reset == 0)
  {
    wait_second(&reset_timer,30);
  }
  else
  {
    if(timeout(&reset_timer)==1)
    {
      reset=0;
      if(state == LED_OFF0 && warm_tmp == 0 && day_tmp == 0 && white_tmp == 0)
      {
        if(ble_connection != 1)
        {
          //Reset BLE module
          GPIOA->ODR &= ~0x08;
        }
        while(1);
      }
    }
  }
  if(timeout(&smooth_time) == 1)
  {
    restart_timer(&smooth_time);
    if(state_mode == LED_MODE_OFF_BEGIN)
    {
      warm_duty = day_duty = white_duty = 0;
      if(warm_tmp == 0 && day_tmp == 0 && white_tmp == 0)
      {
        state_mode = LED_MODE_OFF;
        if(first_reset == 1)
        {
          first_reset = 0;
        }
        else
        {
          if(state == LED_OFF0)
          {
            reset = 1;
          }
        }
      }
    }
    
    warm_tmp = adjust_duty(warm_tmp,warm_duty);
    day_tmp = adjust_duty(day_tmp,day_duty);
    if(finish_change == 1)
    {
      goto finished_change;
    }
    white_tmp = adjust_duty(white_tmp,white_duty);
    /*
    if(white_tmp > white_duty)
    {
      delta_value = 40 + ((white_tmp - white_duty) >> 11);
      if(fixity_brightness)
      {
        if(fixity_delta_value)
        {
          delta_value = fixity_delta_value;
        }
        else
        {
          fixity_delta_value = delta_value;
        }
      }
      white_tmp = white_tmp >= delta_value ? white_tmp - delta_value : 0;
      if(white_tmp < white_duty)
      {
       white_tmp = white_duty;          
      }
    }
    else if(white_tmp < white_duty)
    {
      delta_value = 40 + ((white_duty - white_tmp) >> 11);
      if(warm_tmp<=1000&&white_tmp<=1000&&day_tmp<=1000)
      {
        delta_value = 5;
      }
      if(fixity_brightness)
      {
        if(fixity_delta_value)
        {
          delta_value = fixity_delta_value;
        }
        else
        {
          fixity_delta_value = delta_value;
        }
      }
      white_tmp += delta_value;
      if(white_tmp > white_duty)
      {
        white_tmp = white_duty;
      }
    }
    */
    
finished_change:
    change_duty(day_tmp,DAY_LIGHT_CHANNAL);
    change_duty(white_tmp,WHITE_LIGHT_CHANNAL);
    change_duty(warm_tmp,WARM_LIGHT_CHANNAL);
  }
  
  if(alarm_running == 1)
  {
    if(timeout(&alarm_time) == 1)
    {
      restart_timer(&alarm_time);
      rightnow.mode = MODE_LIGHT_COLOR;
      if(rightnow.light.light_color.warm <= 95)
      {
        rightnow.light.light_color.warm += 5;
        update_duty(rightnow);
      }
      else if(rightnow.light.light_color.day <= 95)
      {
        rightnow.light.light_color.day += 5;
        update_duty(rightnow);
      }
      else if(rightnow.light.light_color.white <= 95)
      {
        rightnow.light.light_color.white += 5;
        update_duty(rightnow);
      }
      
      /*
      if(warm_duty < 10000)
      {
        warm_duty += 500;
        rightnow.light.light_color.warm = warm_duty/100;
      }
      else if(day_duty < 10000)
      {
        day_duty += 500;
        rightnow.light.light_color.day = day_duty/100;
      }
      else if(white_duty < 10000)
      {
        white_duty += 500;
        rightnow.light.light_color.white = white_duty/100;
      }
      */
      else
      {
        alarm_running = 0;
        update_duty(alarm_before);
        state = alarm_state;
      }
    }
  }
  else if(plan_halt==1)
  {
    for(i=0;i<PLAN_MAX_NUM;i++)
    {
      if(min==plan[i].open_min&&hour==plan[i].open_hour
         &&sec==0&&opened[i]!=RUNNING)
      {
        for(j=0;j<PLAN_MAX_NUM;j++)
        {
          if(opened[j]==RUNNING) opened[j]=HALT;
        }
        opened[i]=RUNNING;
        //current_mode = plan[i].mode;
        //update_duty(led[current_mode],plan[i].brightness);
        plan_led.mode = MODE_COLOR_TEMP;
        plan_led.light.light_temp.color_temp = plan[i].color_temp;
        plan_led.light.light_temp.brightness = plan[i].brightness;
        if(plan_record==0)
        {
          plan_record=1;
          plan_before = rightnow;
          plan_state = state;
        }
        update_duty(plan_led);
        state = LED_NORMAL;
      }
    }
    for(i=0;i<PLAN_MAX_NUM;i++)
    {
      if(min==plan[i].close_min&&hour==plan[i].close_hour
         &&sec==0&&opened[i]!=CLOSE)
      {
        if(opened[i]==RUNNING)
        {
            update_duty(plan_before);
            plan_record=0;
            state = plan_state;
        }
        opened[i]=CLOSE;
      }
    }
  }
  
  /*
  else if(plan_running == 1)
  {   
    //Time to close current plan!!!now
    close_time = 60 * plan[current_plan].close_hour + plan[current_plan].close_min; 
    open_time = 60 * plan[current_plan].open_hour + plan[current_plan].open_min; 
    if(close_time >= open_time)
    {
      if(now_time > close_time)
      {
        //warm_duty = day_duty = white_duty = 0;
        update_duty(last);
        plan_running = 0;
        return;
      }
    }
    else
    {
      if(now_time > close_time && now_time < open_time)
      {
        //warm_duty = day_duty = white_duty = 0;
        update_duty(last);
        plan_running = 0;
        return;
      } 
    }     
  }
  else if(plan_running == 0)
  {
    for(i=0;i<PLAN_MAX_NUM;i++)
    {
      
      if(plan_halt[i] == 1)
      {
        continue;
      }
      open_time = 60 * plan[i].open_hour + plan[i].open_min;
      close_time = 60 * plan[i].close_hour + plan[i].close_min;
    
      if((close_time >= open_time && now_time >= open_time && now_time <= close_time)
         ||(close_time < open_time && (now_time >= open_time || now_time <= close_time )))
      {
        plan_running = 1;
        current_plan = i;
        current_mode = plan[current_plan].mode;
        update_duty(led[current_mode]);
        break;
      }
    }
  }
  
  for(i=0;i<PLAN_MAX_NUM;i++)
  {
    if(plan_halt[i] != 1)
    {
      continue;
    }
    open_time = 60 * plan[i].open_hour + plan[i].open_min;
    close_time = 60 * plan[i].close_hour + plan[i].close_min;
    if(close_time >= open_time)
    {
      if(now_time > close_time)
      {
        plan_halt[i] = 0;  
      }
    }
    else
    {
      if(now_time > close_time && now_time < open_time)
      {
        plan_halt[i] = 0;  
      } 
    } 
  }
  */
  if(alarm.halt == 1 && alarm_running == 0)
  {
    if(min == alarm.min && hour == alarm.hour && sec == 0)
    {
      halt_plan(1);
      alarm_running = 1;
      //后面有wait_second函数。。。update_time(&alarm_time);
      wait_second(&alarm_time,alarm.duration);
      alarm_before = rightnow;
      rightnow.mode = MODE_LIGHT_COLOR;
      rightnow.light.light_color.day = rightnow.light.light_color.white = rightnow.light.light_color.warm = 0;
      // 0; 0;
      
      update_duty(rightnow);
      alarm_state = state;
      state = LED_NORMAL;
    }
  }
  if(send_plan)
  {
    if(timeout(&send_plan_time))
    {
      restart_timer(&send_plan_time);
      blecomm_send(0xB3,send_plan_num);
      send_plan_num++;
      if(send_plan_num>=10)
      {
        send_plan = 0;
        send_plan_num = 0;
      }
    }
  }
}

void switch_led(uint8_t led_mode)
{
  
  switch (led_mode)
  {
    case LED_MODE_OFF :
      state_mode = LED_MODE_OFF_BEGIN;
      //warm_duty = day_duty = white_duty = 0; 
      halt_plan(0);
      rightnow = OFF_led;
      update_duty(rightnow);
      //
      fixity_brightness = alarm_running = 0;
      break;
    case LED_MODE_NIGHT:
      /*if(alarm_running == 1)
      {
        alarm_running = 0;
        state = LED_OFF0;
        state_mode = LED_MODE_OFF_BEGIN;
        warm_duty = day_duty = white_duty = 0;
        
        if(last.brightness != 0)
        {
          last_brightness = last.brightness;
          last.brightness = 0;
        }
      }
      else
      {
        state_mode = LED_MODE_NIGHT;
        state = LED_NIGHT;
        update_duty(night);
        last = night;
      }
      fixity_brightness = 0;*/
      if(alarm_running == 1)
      {
        alarm_running = 0;
      }
      state_mode = LED_MODE_NIGHT;
      state = LED_NIGHT;
      halt_plan(0);
      update_duty(night);
      last = night;
      fixity_brightness = 0;
      break;
    case LED_MODE_NORMAL :
      /*
      if(alarm_running == 1)
      {
        alarm_running = 0;
        state = LED_OFF0;
        state_mode = LED_MODE_OFF_BEGIN;
        warm_duty = day_duty = white_duty = 0; 
        
        if(last.brightness != 0)
        {
          last_brightness = last.brightness;
          last.brightness = 0;
        }
      }
      else
      {
        state_mode = LED_MODE_NORMAL;
        last.mode = MODE_LIGHT_COLOR;
        last.light.light_color.warm = 80;
        last.light.light_color.day = 80;
        last.light.light_color.white = 80;
        update_duty(last);
      }
      fixity_brightness = 0;*/
      if(alarm_running == 1)
      {
        alarm_running = 0;
      }
      state_mode = LED_MODE_NORMAL;
      last.mode = MODE_LIGHT_COLOR;
      last.light.light_color.warm = last.light.light_color.day = last.light.light_color.white = 100;
      halt_plan(0);
      update_duty(last);
      fixity_brightness = 0;
      break;
    case LED_OFF :
      state = LED_OFF0;
      state_mode = LED_MODE_OFF_BEGIN;
      //warm_duty = day_duty = white_duty = 0; 
      /*
      if(last.brightness != 0)
      {
        last_brightness = last.brightness;
        last.brightness = 0;
      }*/
      halt_plan(0);
      rightnow = OFF_led;
      update_duty(rightnow);
      fixity_brightness = 0;
      alarm_running = 0;
      break;
    case LED_MODE_LAST :
      state = LED_NORMAL;
      state_mode = LED_MODE_LAST;
      //last.brightness = last_brightness;
      halt_plan(0);
      update_duty(last);
      fixity_brightness = 0;
      alarm_running = 0;
      break;
  }
}

void turn_off_led(void)
{
  warm_duty = 0;
  day_duty = 0;
  white_duty = 0;
  warm_tmp >>= 7;
  day_tmp >>= 7;
  white_tmp >>= 7;
  /*
  change_duty(day_tmp,DAY_LIGHT_CHANNAL);
  change_duty(white_tmp,WHITE_LIGHT_CHANNAL);
  change_duty(warm_tmp,WARM_LIGHT_CHANNAL);
  asm("sim");
  //PC4
  EXTI->CR1 = 0;
  //turn off reg in halt mode
  CLK->ICKR |= CLK_ICKR_SWUAH;
  //turn off flash in halt mode
  FLASH->CR1 |= FLASH_CR1_AHALT;
  //enable the interrupt of PC4 
  GPIOC->CR2 |= 0x10;
  asm("rim");
  asm("halt");
  GPIOC->CR2 &= ~0x10;*/
}

#pragma vector=0x07
__interrupt void GPIOCHandlerInterrupt (void)
{
  tmp++;
  GPIOC->CR2 &= ~0x10;
}
