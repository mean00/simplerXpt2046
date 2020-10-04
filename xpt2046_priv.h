#pragma once

#define NB_BLOCKS 10



#define CHANNEL_Z1 (3<<4) //b1 3
#define CHANNEL_Z2  (4<<4) //c1 4
#define CHANNEL_Y (1<<4) //91 1
#define CHANNEL_X  (5<<4) //d1 5

#define CHANNEL(x)  (x<<4)
// 12 bits ADC hardcoded, bit is at 0 for that
#define REF_OFF_ADC_ON 1

#define XPT_CMD(x) (0x80+REF_OFF_ADC_ON+x)

static const uint8_t READ_SEQUENCE[]=
{
  XPT_CMD(CHANNEL_Z1),0, // b1   0
  XPT_CMD(CHANNEL_Z2),0, // c1   1
  
  XPT_CMD(CHANNEL_X),0, // 91    2
  XPT_CMD(CHANNEL_Y),0, // c1    3
  

  XPT_CMD(CHANNEL_X),0, // 91    4 
  XPT_CMD(CHANNEL_Y),0, // d1    5
  
  XPT_CMD(CHANNEL_X),0, // 91    6
  XPT_CMD(CHANNEL_Y),0, // d1    7
  
  XPT_CMD(CHANNEL_X),0, // 91    8
  XPT_CMD(CHANNEL_Y)&0xf7,0, // d0   9
  
  0,0
};

// EOF