#include"eeprom.h"
#include"hw.h"

uint8_t write_eeprom(uint8_t *addr,uint8_t *pdata,uint8_t len)
{
  uint8_t i;
  uint8_t tmp;
  if((uint16_t)addr < 0x4000 || (uint16_t)addr > 0x407F || (uint16_t)addr + len > 0x407F)
  {
    return 0xFF;
  }
  feed_dog();
  asm("sim");
  //unlock eeprom
  tmp = FLASH->IAPSR;
  while(!(tmp & FLASH_IAPSR_DUL))
  {
   FLASH->DUKR = 0xAE;//KEY1:
   FLASH->DUKR = 0x56;//KEY2:
   tmp = FLASH->IAPSR ;
  }
  for(i=0;i<len;i++)
  {
    addr[i] = pdata[i];
    while(!(FLASH->IAPSR & FLASH_IAPSR_EOP));
  }
  //locked eeprom  
  FLASH->IAPSR &= ~FLASH_IAPSR_DUL;
  asm("rim");
  return 1;
}