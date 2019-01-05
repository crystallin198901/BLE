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
����void DS1302WriteOneWord(unsigned char addr, unsigned char data)
������DS1302��addr��ַд��data������
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
����unsigned char DS1302ReadOneWord(unsigned char addr)
���ڴ�DS1302�е�addr��ַ��ȡ����
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
����void DS1302WriteTimeData(void)������DS1302
д��ʱ����������
*****************************************************/
void DS1302WriteTimeData(void)
{
  DS1302WriteOneWord(0x8E,0x00);
  //�ر�д����
  DS1302WriteOneWord(0x80,0x80);
  //�ر�ʱ��
  DS1302WriteOneWord(0x8C,bin2bcd(year));
  //д�����
  DS1302WriteOneWord(0x88,bin2bcd(month));
  //д���·�
  DS1302WriteOneWord(0x86,bin2bcd(day));
  //д����
  //DS1302WriteOneWord(0x8A,bin2bcd(week));
  //д������
  DS1302WriteOneWord(0x84,bin2bcd(hour));
  //д��Сʱ
  DS1302WriteOneWord(0x82,bin2bcd(min));
  //д�����
  DS1302WriteOneWord(0x80,bin2bcd(sec));
  //д���룬����ʱ��
  DS1302WriteOneWord(0x8E,0x80);
  //����д����
}
/*****************************************************
����void DS1302ReadTimeData(void)���ڴ�DS1302��
��ȡʱ����������
*****************************************************/
void DS1302ReadTimeData(void)
{
  DS1302WriteOneWord(0x8E,0x00);
  //�ر�д����
  year=bcd2bin(DS1302ReadOneWord(0x8D));
  //��ȡ���
  month=bcd2bin(DS1302ReadOneWord(0x89));
  //��ȡ�·�
  day=bcd2bin(DS1302ReadOneWord(0x87));
  //��ȡ��
    week=DS1302ReadOneWord(0x91);
    //DS1302WriteOneWord(0x90,0x5B);
    //��ȡ����
  hour=bcd2bin(DS1302ReadOneWord(0x85));
  //��ȡСʱ
  min=bcd2bin(DS1302ReadOneWord(0x83));
  //��ȡ����
  sec=DS1302ReadOneWord(0x81);
  sec&=~0x80;
  sec=bcd2bin(sec);
  //��ȡ��
  DS1302WriteOneWord(0x8E,0x80);
  //����д����
  //DS1302WriteOneWord(0x90,0x00);
  //��ֹ������
}
