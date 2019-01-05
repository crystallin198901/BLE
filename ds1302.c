#include"ds1302.h"
uint8_t year=17;
uint8_t month=3;
uint8_t day=13;
uint8_t hour=8;
uint8_t min=29;
uint8_t sec=0;
uint8_t week=0;

void delay(uint8_t temp)
{
    while(--temp);
}

uint8_t bcd2bin(uint8_t n)
{
  return n/16*10+n%16 ;
}

uint8_t bin2bcd(uint8_t n)
{
	return n/10*16+n%10 ;
}
/*****************************************************
函数void DS1302WriteOneWord(unsigned char addr, unsigned char data)
用于向DS1302的addr地址写入data的数据
*****************************************************/
void DS1302WriteOneWord(uint8_t addr,uint8_t data)
{
  uint8_t i ;
	InitDS1302ClockPort;
	InitDS1302ChipEnablePort;
	SetDS1302DataPortWriteMode ;
  DS1302Rst_0 ;
  delay(2);
  DS1302Sclk_0 ;
  delay(2);
  DS1302Rst_1 ;
  delay(2);
  for(i=8;i>0;i--)
  {
      
    if(addr&0x01)
    {
      DS1302Data_1 ;
    }
    else 
    {
      DS1302Data_0 ;
    }
    delay(2);
    DS1302Sclk_1 ;
    delay(2);
    DS1302Sclk_0 ;
    delay(2);
    addr=addr>>1 ;
  }
    
  for(i=8;i>0;i--)
  {
        
    if(data&0x01)
    {
      DS1302Data_1 ;
    }
    else 
    {
      DS1302Data_0 ;
    }
    delay(2);
    DS1302Sclk_1 ;
    delay(2);
    DS1302Sclk_0 ;
    delay(2);
    data=data>>1 ;
  }
	    
  DS1302Rst_0 ;	
  delay(2);
}
/*****************************************************
函数unsigned char DS1302ReadOneWord(unsigned char addr)
用于从DS1302中的addr地址读取数据
*****************************************************/
uint8_t DS1302ReadOneWord(uint8_t addr)
{
	uint8_t i ;
	uint8_t data ;
	InitDS1302ClockPort;
	InitDS1302ChipEnablePort;
	SetDS1302DataPortWriteMode;
  DS1302Rst_0 ;
  delay(2);
  DS1302Sclk_0 ;
  delay(2);
  DS1302Rst_1 ;
  delay(2);
  for(i=8;i>0;i--)
  {
      
    if(addr&0x01)
    {
      DS1302Data_1 ;
    }
    else 
    {
      DS1302Data_0 ;
    }
    delay(2);
    DS1302Sclk_1 ;
    delay(2);
    DS1302Sclk_0 ;
    delay(2);
    addr=addr>>1 ;
	}
    
  SetDS1302DataPortReadMode ;
  //GPIOB->DDR &= ~0x10;
  //GPIOB->CR1 |= 0x10;GPIOB->CR2 &= ~0x10;
  data=0 ;
   
  for(i=8;i>0;i--)
  {
    data=data>>1 ;
       
    if(DS1302Data)
    {
      data|=0x80 ;
    }
        
    DS1302Sclk_1 ;
    delay(2);
    DS1302Sclk_0 ;
    delay(2);
  }
    
  DS1302Rst_0 ;	
  delay(2);
  return(data);
   
}
/*****************************************************
函数void DS1302WriteTimeData(void)用于向DS1302
写入时钟日期数据
*****************************************************/
void DS1302WriteTimeData(void)
{
  DS1302WriteOneWord(0x8E,0x00);
  //关闭写保护
  DS1302WriteOneWord(0x80,0x80);
  //关闭时钟
  DS1302WriteOneWord(0x8C,bin2bcd(year));
  //写入年份
  DS1302WriteOneWord(0x88,bin2bcd(month));
  //写入月份
  DS1302WriteOneWord(0x86,bin2bcd(day));
  //写入日
  //DS1302WriteOneWord(0x8A,bin2bcd(week));
  //写入星期
  DS1302WriteOneWord(0x84,bin2bcd(hour));
  //写入小时
  DS1302WriteOneWord(0x82,bin2bcd(min));
  //写入分钟
  DS1302WriteOneWord(0x80,bin2bcd(sec));
  //写入秒，开启时钟
  DS1302WriteOneWord(0x8E,0x80);
  //开启写保护
}
/*****************************************************
函数void DS1302ReadTimeData(void)用于从DS1302中
读取时钟日期数据
*****************************************************/
void DS1302ReadTimeData(void)
{
  DS1302WriteOneWord(0x8E,0x00);
  //关闭写保护
  year=bcd2bin(DS1302ReadOneWord(0x8D));
  //读取年份
  month=bcd2bin(DS1302ReadOneWord(0x89));
  //读取月份
  day=bcd2bin(DS1302ReadOneWord(0x87));
  //读取日
    week=DS1302ReadOneWord(0x91);
    //DS1302WriteOneWord(0x90,0x5B);
    //读取星期
  hour=bcd2bin(DS1302ReadOneWord(0x85));
  //读取小时
  min=bcd2bin(DS1302ReadOneWord(0x83));
  //读取分钟
  sec=DS1302ReadOneWord(0x81);
  sec&=~0x80;
  sec=bcd2bin(sec);
  //读取秒
  DS1302WriteOneWord(0x8E,0x80);
  //开启写保护
  //DS1302WriteOneWord(0x90,0x00);
  //禁止涓流充电
}
