#include"blecomm.h"
static char tx_buf[TX_BUF_LEN]={0};
static char rx_buf[RX_BUF_LEN]={0};
uint8_t normal_reset = 0;
/*
const char at_commd[]="TTM:";
const char OK_CMD[]="TTM:OK";
const char E
RR_CMD[]="ERP";
const char at_mac_return[]="TTM:MAC-";
const char at_mac_commd[]="TTM:MAC-?";*/
//const char at_ren_commd[]="TTM:REN-";

/*
char at_commd[]="TTM:";
char OK_CMD[]="TTM:OK";
char ERR_CMD[]="ERP";
char at_mac_return[]="TTM:MAC-";
char at_mac_commd[]="TTM:MAC-?";
char at_ren_commd[]="TTM:REN-";
*/
time_t rx_wait;
uint8_t Rx_num=0,Rx_len=0,tmp_Rx_num=0;
uint8_t Tx_num=0,Tx_len=0;
//static uint8_t cmd_state = 0;
//static uint8_t MAC_addr[12];
//static time_t cmd_time;
//static uint8_t rename_process=0;
__no_init uint8_t ble_connection;
static time_t flash_time;
static time_t BLE_reset_timer;
uint8_t BLE_reset_flag=1;

extern led_t last;
extern uint8_t year;
extern uint8_t month;
extern uint8_t day;
extern uint8_t hour;
extern uint8_t min;
extern uint8_t sec;
extern uint8_t update;
extern led_t night;
extern led_t night_eeprom;
extern alarm_t alarm;
extern alarm_t alarm_eeprom;
extern uint8_t plan_halt_eeprom;
//extern led_t led[5];
//extern led_t led_eeprom[5];
extern plan_t plan[5];
extern plan_t plan_eeprom[5];
extern uint8_t fixity_brightness;
extern uint8_t uc_warm,uc_day,uc_white;
extern uint8_t state;
extern led_t rightnow;
extern time_t send_plan_time;
extern uint8_t send_plan;
extern uint8_t send_plan_num;
extern uint8_t plan_halt;

void init_blecomm(void)
{
  uint8_t tmp;
  tmp = RST->SR;
  
  if((RST->SR & 0x03) == 0)
  {
    ble_connection = 0;
    last = night_eeprom;
    normal_reset = 0;
  }
  //看门狗复位
  else
  {
    normal_reset = 1;
  }
  RST->SR = tmp;
  CLK->PCKENR1 |= 0X08;
  UART1->SR = 0;
  GPIOD->DDR |= 0x20;
  GPIOD->ODR |= 0x20;
  GPIOD->CR1 |= 0x20;
  //Baudrate = 9600
  //UART1->BRR2 = 0x00;
  //UART1->BRR1 = 0X0D;
  UART1->BRR2 = 0x00;
  UART1->BRR1 = 0x30;
  /*UART1->BRR2 = 0x01;
  UART1->BRR1 = 0X34;*/
  /*UART1->BRR2 = 0x08;
  UART1->BRR1 = 0x68;16mhz*/
  UART1->CR2 =  UART2_CR2_REN | UART1_CR2_RIEN;
  //后面有wait_second函数。。。update_time(&rx_wait);
  wait_ms(&rx_wait,5);
  wait_second(&BLE_reset_timer,1800);
  wait_second(&flash_time,1);
}

void reinit_uart(void)
{
  CLK->PCKENR1 |= 0X08;
  /*UART1->BRR2 = 0x01;
  UART1->BRR1 = 0X34;*/
  UART1->BRR2 = 0x00;
  UART1->BRR1 = 0x30;
  if(Tx_len == 0 && Rx_num == 0)
  {
    UART1->CR2 =  UART2_CR2_REN | UART1_CR2_RIEN;
  }
  
}

#pragma vector=0x13
__interrupt void UART1TxHandlerInterrupt (void)
{
  UART1->SR &= ~UART1_SR_TC;
  Tx_num++;
  if(Tx_num < Tx_len)
  {
    UART1->DR = tx_buf[Tx_num];
  }
  else
  {
    Tx_len = 0;
    Tx_num = 0;
    UART1->CR2 &= ~(UART1_CR2_TCIEN | UART1_CR2_TEN);
  }
}
#pragma vector=0x14
__interrupt void UART1RxHandlerInterrupt (void)
{
  uint8_t tmp;
  UART1->SR &= ~(UART1_SR_OR | UART1_SR_RXNE);
  if(Rx_len == 0)
  {
    rx_buf[Rx_num] = UART1->DR;
    Rx_num++;
    if(Rx_num >= 100)
    {
      Rx_num = 0;
      Rx_len = 100;
      return;
    }
  }
  else
  {
    tmp = UART1->DR;
  }
  return;
}

/*
uint8_t AT_commd_deal(void)
{
  uint8_t i,j;
  for(i=0;i<4;i++)
  {
    if(at_commd[i] != rx_buf[i])
    {
      return 1;
    }
  }
    switch (rx_buf[4])
    {
      case 'O' :
        i=0;
        while(OK_CMD[i])
        {
          if(rx_buf[i+4] != OK_CMD[i])
          {
            return 1;
          }
          i++;
        }
        if(cmd_state)
        {
          cmd_state = 0;
        }
        break;
      case 'E' :
        i=0;
        while(at_mac_commd[i])
        {
          if(rx_buf[i] != at_mac_commd[i])
          {
            return 1;
          }
          i++;
        }
        
        break;
      case 'M' :
        i=0;
        while(at_mac_commd[i])
        {
          if(rx_buf[i] != at_mac_commd[i])
          {
            return 1;
          }
          i++;
        }
        if(rx_buf[i++] != 0x0D && rx_buf[i++] != 0x0A)
        {
          return 1;
        }
        i=0;
        while(at_mac_return[i])
        {
          if(rx_buf[11+i] != at_mac_return[i])
          {
            return 1;
          }
          i++;
        }
        for(i=0;i<12;i++)
        {
          MAC_addr[i] = rx_buf[19+i];
        }
        if(cmd_state)
        {
          cmd_state = 0;
        }
        if(rename_process)
        {
          blecomm_send(0xC2,0);
          rename_process = 0;
        }
        break;
      case 'R' :
        i = 0;
        while(at_ren_commd[i])
        {
          if(rx_buf[i] != at_ren_commd[i])
          {
            return 1;
          }
          i++;
        }
        while(rx_buf[i] != 0x0D || rx_buf[i+1] != 0x0A)
        {
          i++;
          if(i >= Rx_len)
          {
            return 1;
          }
        }
        i++;
        for(j=0;j<6;j++)
        {
          i++;
          if(rx_buf[i] != OK_CMD[j])
          {
            return 1;
          }
        }
        if(cmd_state)
        {
          cmd_state = 0;
        }
        break;
    }
  return 0;
}*/
void blecomm_handler(void)
{
  uint8_t sum_crc,xor_crc,i;
  //led_t led_tmp;
  //unsigned long a;
  //reset the BLE module after 30min whitout any operation
  if(timeout(&BLE_reset_timer)==1)
  {
    if(BLE_reset_flag == 1)
    {
      //Reset BLE module
      GPIOA->ODR &= ~0x08;
      BLE_reset_flag = 0;
      wait_second(&BLE_reset_timer,1);
    }
    else
    {
      GPIOA->ODR |= 0x08;
      BLE_reset_flag = 1;
      wait_second(&BLE_reset_timer,1800);
    }
  }
  if(timeout(&flash_time)!=0)
  {
    if(ble_connection!=1)
    {
       if(GPIOD->IDR & 0x04)
       {
         GPIOD->ODR &= ~0x04;
         wait_second(&flash_time,3);
       }
       else
       {
         GPIOD->ODR |= 0x04;
         wait_second(&flash_time,1);
       }
    }
    else
    {
      GPIOD->ODR |= 0x04;
      wait_second(&flash_time,2);
    }
  }
  if(Rx_num != 0)
  {
    if(tmp_Rx_num != Rx_num)
    {
      tmp_Rx_num = Rx_num;
      restart_timer(&rx_wait);
    }
    //超过20ms没有收到新的字节
    if(timeout(&rx_wait))
    {
      Rx_len = Rx_num;
      Rx_num = 0;
      tmp_Rx_num = 0;
    }
  }
  else
  {
    restart_timer(&rx_wait);
  }
  if(Rx_len != 0)
  {
    if(rx_buf[0]=='T' && rx_buf[1]=='T' && rx_buf[2]=='M')
    {
      if(rx_buf[4]=='O' && rx_buf[5]=='K' 
         && rx_buf[6]=='\r' && rx_buf[7]=='\n')
      {
        ble_connection = 1;
      }
      else if(rx_buf[4]=='D' && rx_buf[5]=='I' && rx_buf[6]=='S' 
              && rx_buf[7]=='C' && rx_buf[8]=='O' && rx_buf[9]=='N'
              && rx_buf[10]=='N' && rx_buf[11]=='E' && rx_buf[12]=='T')
      {
        ble_connection = 0;
      }
      /*
      Rx_len = 0;
      Rx_num = 0;
      */
    }
    else
    {
      if(Rx_len<2)
      {
        goto finished;
      }
      sum_crc = xor_crc = 0;
      for(i=0;i<Rx_len-2;i++)
      {
        sum_crc += rx_buf[i];
        xor_crc ^= rx_buf[i];
      }
      if(sum_crc != rx_buf[Rx_len-2] || xor_crc != rx_buf[Rx_len-1])
      {
        Rx_len = 0;
        Rx_num = 0;
        return;
      }
      if(BLE_reset_flag==1)  
        wait_second(&BLE_reset_timer,1800);
      switch (rx_buf[0])
      {
        //switch off/on the led
        case 0xA1 :
          if(rx_buf[1] == 0)
          { 
            if(state != LED_OFF0 || state != LED_OFF2) switch_led(LED_OFF);
          }
          else 
          {
            if(state == LED_OFF0 || state == LED_OFF2) 
              switch_led(LED_MODE_LAST);
          }
          break;
        //set the brightness
        case 0xA2 :
          last.mode = MODE_COLOR_TEMP;
          last.light.light_temp.color_temp = 50*rx_buf[1]+2200;
          last.light.light_temp.brightness = rx_buf[2];
          update = 1;
          fixity_brightness = 0;
          break;
        //set the value to channel
        case 0xA3 :
          /*
          if(last.mode == MODE_LIGHT_COLOR)
          {
          }
          else
          {
            if(last.light.color_temp < 4000)
            {
              a = 10000 * (unsigned long)(last.light.color_temp - 2200) / (4000 - 2200);
              last.light.light_color.day = (uint16_t)a;
              last.light.light_color.warm = 10000 - last.light.light_color.day;
              last.light.light_color.white = 0;
            }
            else
            {
              a = 10000 * (unsigned long)(6500 - last.light.color_temp) / (6500 - 4000);
              last.light.light_color.day = (uint16_t)a;
              last.light.light_color.white = 10000 - last.light.light_color.day;
              last.light.light_color.warm = 0;
            }
            last.mode = MODE_LIGHT_COLOR;
          }*/
          if(rightnow.mode==MODE_COLOR_TEMP)
          {/*
            last.light.light_color.warm = rightnow.light.light_color.warm;
            last.light.light_color.day = rightnow.light.light_color.day;
            last.light.light_color.white = rightnow.light.light_color.white;*/
            last.light.light_color.warm = uc_warm;
            last.light.light_color.day = uc_day;
            last.light.light_color.white = uc_white;
          }
          last.mode = MODE_LIGHT_COLOR;
          if(rx_buf[1] == 1)
          {
            last.light.light_color.warm = rx_buf[2];
          }
          else  if(rx_buf[1] == 2)
          {
            last.light.light_color.day = rx_buf[2];
          }
          else if(rx_buf[1] == 3)
          {
            last.light.light_color.white = rx_buf[2];
          }
          update = 1;
          fixity_brightness = 0;
        break;
        //set color temperature
        case 0xA4 :
          /*
          last.light.light_color.warm = 0;
          last.light.light_color.day = 0;
          last.light.light_color.white = 0;*/
          last.light.light_temp.color_temp = 50 * (uint16_t)rx_buf[1] + 2200;
          last.mode = MODE_COLOR_TEMP;
          update = 1;
          fixity_brightness = 1;
          break;
        //set light color
        case 0xA5 :
          last.mode = MODE_LIGHT_COLOR;
          last.light.light_color.warm = rx_buf[1];
          last.light.light_color.day = rx_buf[2];
          last.light.light_color.white = rx_buf[3];
          //last.brightness = rx_buf[4];
          update = 1;
          fixity_brightness = 0;
          break;
        case 0xA6 :
          if(rx_buf[1]==0x55)
          {
            blecomm_send(0xA6,0);
          }
          break;
        //set night mode
        case 0xB1 :
          if(rx_buf[1] == 1)
          {
            night.mode = MODE_COLOR_TEMP;
            night.light.light_temp.color_temp = 2200 + 50 * (uint16_t)rx_buf[3];
            night.light.light_temp.brightness = rx_buf[2];
            write_eeprom((uint8_t *)&night_eeprom,(uint8_t *)&night,sizeof(night));
          }
          else 
          {
            blecomm_send(0xB1,0);
          }
          break;
        //set alarm time
        case 0xB2:
          if(rx_buf[1] == 1)
          {
            write_eeprom((uint8_t *)&alarm_eeprom,(uint8_t *)&rx_buf[2],sizeof(alarm_t));
            alarm = alarm_eeprom;
            if(alarm.halt==0)
            {
              halt_alarm();
            }
          }
          else
          {
            blecomm_send(0xB2,0);
          }
          break;
        //change the plan
        case 0xB3 :
          i = rx_buf[2] - 1;    
          //rx_buf[7]--;
          if(rx_buf[1] == 1)
          {
            if(i>9) break;
            if(i==5)
            {
              plan[i].color_temp = (uint16_t)rx_buf[7]*50+2200;
            }
            plan[i].open_hour = rx_buf[3];
            plan[i].open_min = rx_buf[4];
            plan[i].close_hour = rx_buf[5];
            plan[i].close_min = rx_buf[6];
            plan[i].color_temp = (uint16_t)rx_buf[7]*50+2200;
            plan[i].brightness = rx_buf[8];
            write_eeprom((uint8_t *)&plan_eeprom[i],(uint8_t *)&plan[i],sizeof(plan_t));  
          }
          else
          {
            send_plan = 1;
            send_plan_num = 0;
            //后面有wait_second函数。。。update_time(&send_plan_time);
            wait_ms(&send_plan_time,100);
            //blecomm_send(0xB3,i);
          }
          break;
        //syn time
        case 0xB4 :
          year = rx_buf[1];
          month = rx_buf[2];
          day = rx_buf[3];
          hour = rx_buf[4];
          min = rx_buf[5];
          sec = rx_buf[6];
          DS1302WriteTimeData();
          break; 
        case 0xB5 :
          if(rx_buf[1]==1)
          {
            if(rx_buf[2]!=plan_halt_eeprom)
            {
              write_eeprom((uint8_t*)&plan_halt_eeprom,(uint8_t*)&rx_buf[2],sizeof(uint8_t));
              if(rx_buf[2]==0) halt_plan(1);
              plan_halt = plan_halt_eeprom;
            }
          }
          else
          {
            blecomm_send(0xB5,0);
          }
          break;
        /*
        case 0xB5 :
          if(rx_buf[2]==0||rx_buf[2]>PLAN_MAX_NUM)
          {
            break;
          }
          rx_buf[2]--;
          if(rx_buf[1] == 1)
          {
            if(rx_buf[3]==MODE_COLOR_TEMP)
            {
              led_tmp.mode = MODE_COLOR_TEMP;
              led_tmp.light.color_temp = rx_buf[4];
              led_tmp.light.color_temp <<= 8;
              led_tmp.light.color_temp |= rx_buf[5];
              led_tmp.brightness = rx_buf[6];
            }
            else if(rx_buf[3]==MODE_LIGHT_COLOR)
            {
              led_tmp.mode = MODE_LIGHT_COLOR;
              led_tmp.light.light_color.warm = rx_buf[4];
              led_tmp.light.light_color.day = rx_buf[5];
              led_tmp.light.light_color.white = rx_buf[6];
              led_tmp.brightness = rx_buf[7]; 
            }
            else
            {
              break;
            }
            led[rx_buf[2]]=led_tmp;
            write_eeprom((uint8_t*)&led_eeprom[rx_buf[2]],(uint8_t*)&led[rx_buf[2]],sizeof(led_t));  
          }
          else if(rx_buf[1] == 0)
          {
            blecomm_send(0xB5,rx_buf[2]);
          }
          break;
        */
      }
    }
  finished:
    for(i=0;i<=Rx_len;i++)
    {
      rx_buf[i] = 0;
    }
    Rx_len = 0;
    Rx_num = 0;
  }
  /*
  if(cmd_state)
  {
    if(timeout(&cmd_time) == 1)
    {
      restart_timer(&cmd_time);
      blecomm_send(cmd_state,0);
    }
  }*/
}

/*
void init_name(void)
{
  blecomm_send(0xC1,0);
  //rename device after rx the mac address
  rename_process = 1;
}
*/
void blecomm_send(uint8_t cmd,uint8_t parameter)
{
  uint8_t i,sum_crc,xor_crc;
  while(Tx_len) UART1->CR2 |= UART1_CR2_TEN | UART1_CR2_TCIEN;
  switch (cmd)
  {
    //get mac address
    /*
    case 0xC1 :
      i = 0;
      while(at_mac_commd[i])
      {
        tx_buf[i] = at_mac_commd[i];
        i++;
      }
      Tx_len = i;
      cmd_state = 0xC1;
      update_time(&cmd_time);
      wait_ms(&cmd_time,10000);
      break;
    //rename
    case 0xC2 :
      i = 0;
      while(at_ren_commd[i])
      {
        tx_buf[i] = at_ren_commd[i];
        i++;
      }
      tx_buf[i++] = 'A';
      tx_buf[i++] = 'G';
      tx_buf[i++] = 'U';
      tx_buf[i++] = 'l';
      tx_buf[i++] = 'a';
      tx_buf[i++] = 'm';
      tx_buf[i++] = 'p';
      tx_buf[i++] = '1';
      
      for(j=0;j<12;j++)
        tx_buf[i++] = MAC_addr[j];
      
      Tx_len = i;
      cmd_state = 0xC2;
      update_time(&cmd_time);
      wait_ms(&cmd_time,10000);
      break;
    */
    /*
    case 0xA1 :
      break;
    case 0xA3 :
      tx_buf[1] = parameter;
      switch (parameter)
      {
        case 1 :
          tx_buf[2] = warm;
          break;
        case 2 :
          tx_buf[2] = day;
          break;
        case 3 :
          tx_buf[2] = white;
          break;
      }
      Tx_len = 3
      break;
    case 0xA5 :
      tx_buf[1] = warm;
      tx_buf[2] = day;
      tx_buf[3] = white;
      Tx_len = 4;
      break;
    */
    case 0xA6 :
      tx_buf[1]= state==LED_OFF0 || state==LED_OFF2 ? 0 : 1;
      if(rightnow.mode==MODE_COLOR_TEMP) 
      {
        tx_buf[2]=1;
        tx_buf[3]=(rightnow.light.light_temp.color_temp - 2200) / 50;
        tx_buf[4]=rightnow.light.light_temp.brightness;
        Tx_len = 5;
      }
      else 
      {
        tx_buf[2] = 2;
        tx_buf[3] = rightnow.light.light_color.warm;
        tx_buf[4] = rightnow.light.light_color.day;
        tx_buf[5] = rightnow.light.light_color.white;
        Tx_len = 6;
      }
      break;
    case 0xB1 :
      tx_buf[0] = 0xB1;
      tx_buf[1] = 0;
      tx_buf[2] = night_eeprom.light.light_temp.brightness;
      tx_buf[3] = (night_eeprom.light.light_temp.color_temp - 2200)/50;
      Tx_len = 4;
      break;
    case 0xB2 :
      tx_buf[0] = 0xB2;
      tx_buf[1] = 0;
      tx_buf[2] = alarm_eeprom.hour;
      tx_buf[3] = alarm_eeprom.min;
      tx_buf[4] = alarm_eeprom.duration;
      tx_buf[5] = alarm_eeprom.halt;
      Tx_len = 6;
      break;
    case 0xB3 :
      tx_buf[0] = 0xB3;
      tx_buf[1] = 0;
      tx_buf[2] = parameter+1;
      tx_buf[3] = plan[parameter].open_hour;
      tx_buf[4] = plan[parameter].open_min;
      tx_buf[5] = plan[parameter].close_hour;
      tx_buf[6] = plan[parameter].close_min;
      tx_buf[7] = (plan[parameter].color_temp-2200)/50;
      tx_buf[8] = plan[parameter].brightness;
      Tx_len = 9;
      break;
    case 0xB5 :
      tx_buf[1] = 0;
      tx_buf[2] = plan_halt_eeprom;
      Tx_len = 3;
      break;
      /*
    case 0xB5 :
      tx_buf[0] = 0xB5;
      tx_buf[1] = 0;
      tx_buf[2] = parameter+1;
      tx_buf[3] = led[parameter].mode;
      if(tx_buf[3]==MODE_COLOR_TEMP)
      {
        tx_buf[4] = (led[parameter].light.color_temp>>8)&0xFF;
        tx_buf[5] = led[parameter].light.color_temp;
        tx_buf[6] = led[parameter].brightness;
        Tx_len = 7;
      }
      else if(tx_buf[3]==MODE_LIGHT_COLOR)
      {
        tx_buf[4]=led[parameter].light.light_color.warm;
        tx_buf[5]=led[parameter].light.light_color.day;
        tx_buf[6]=led[parameter].light.light_color.white;
        tx_buf[7] = led[parameter].brightness;
        Tx_len = 8;
      }
      break;
      */
  }
  if(cmd < 0xC0)
  {
    tx_buf[0]=cmd;
    sum_crc = xor_crc = 0;
    for(i=0;i<Tx_len;i++)
    {
      sum_crc += tx_buf[i];
      xor_crc ^= tx_buf[i];
    }
    tx_buf[i++] = sum_crc;
    tx_buf[i++] = xor_crc;
    Tx_len += 2;
  }
  for(i=Tx_len;i<TX_BUF_LEN;i++)
    tx_buf[i] = 0;
  Tx_num = 0;
  UART1->SR &= ~UART1_SR_TC;
  UART1->CR2 |= UART1_CR2_TEN | UART1_CR2_TCIEN;
  UART1->DR = tx_buf[Tx_num]; 
}