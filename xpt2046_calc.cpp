/**
 * 
 * @param spi
 * @param cs
 * @param speed
 * @param x
 */
#pragma once
#include "xpt2046_impl.h"
#include "dso_debug.h"

#define NB_BLOCKS 10




#define CHANNEL_Z1 (3<<4) //b1 3
#define CHANNEL_Z2  (4<<4) //c1 4
#define CHANNEL_Y (1<<4) //91 1
#define CHANNEL_X  (5<<4) //d1 5

#define CHANNEL(x)  (x<<4)
// 12 bits ADC hardcoded, bit is at 0 for that
#define REF_OFF_ADC_ON 1

#define XPT_CMD(x) (0x80+REF_OFF_ADC_ON+x)
/**
 */
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
  XPT_CMD(CHANNEL_Y)&0xfe,0, // d0   9
  
  0,0
};


/**
 * 
 * @return 
 */

static uint8_t rx[32];  
/**
 * 
 * @param a
 * @param b
 * @param c
 * @return 
 */
int XPT2046impl::median(int aa, int bb, int cc,int dd)
{
    int val[4]={
        mRawData[aa],
        mRawData[bb],
        mRawData[cc],
        mRawData[dd]
    };
    
    int sum=0;
    int mx=0;
    int mn=0xf000;
    
    for(int i=0;i<4;i++)
    {
        int v=val[i];
        sum+=v;
        if(v<mn) mn=v;
        if(v>mx) mx=v;
    }
    return (sum-mn-mx)/2;
}

/**
 * 
 * @return 
 */
int xmax=0,ymax=0;

bool XPT2046impl::rawRead(int &x, int &y)
{    
     mTex->lock();
     mSPI.beginTransaction(mSettings );
     digitalWrite(mCs,LOW);       

     int sz=sizeof(READ_SEQUENCE);
     mSPI.dmaTransfer(READ_SEQUENCE,rx,sz);

     digitalWrite(mCs, HIGH);
     mSPI.endTransaction();
     mTex->unlock();

     // convert readings from buffer into human readable form    
     uint8_t *b=rx+1;
     for(int i=0;i<NB_BLOCKS;i++)
     {
         uint32_t c=(b[0]<<8)+(b[1]);
         b+=2;
         c>>=3; // remove the 3 non significant bits
         mRawData[i]=c;
     }        

     // Resend init sequence
     

     int z1=mRawData[0];
     int z2=mRawData[1];     
     
     if(z1>4000 || z2 >4000) return false;
     int z=z1+4095-z2;
     if(z<0) z=0;
     if(z<400)
         return false;
 //   Logger("Z %d  (%d/%d)\n",z,  z1, z2);
        
    if(1) // Swap x/y
    {
     y=median(2,4,6,8);
     x=median(3,5,7,9);
    }
    else
    {
     x=median(2,4,6,8);
     y=median(3,5,7,9);
    }
     
     return true;

}
/**
 * 
 * @param value
 * @param fromBegin
 * @param fromEnd
 * @param toBegin
 * @param toEnd
 * @return 
 */
static int xmap(int value, int fromBegin, int fromEnd, int toBegin, int toEnd)
{
    bool swap=false;
    if(fromEnd<fromBegin)
    {
        swap=true;
        int z=fromEnd;
        fromEnd=fromBegin;
        fromBegin=z;
    }
    int out=map(value,fromBegin,fromEnd,toBegin-25,toEnd+25);
    if(swap) out=toEnd-out;
    if(out>=toEnd) out=toEnd-1;
    if(out<0) out=0;
    return out;
}
// in 10 ms tick
#define SINGLE_THRESHOLD 10
#define HOLDOFF          5

/**
 * 
 */
void     XPT2046impl::run()
{
    // load calibration data
    xAssert(mCalibration);
#define CALX(p)    mCalibration[p*2+0]
#define CALY(p)    mCalibration[p*2+1]
    int xmin=(CALX(0)+CALX(3))/2;
    int xmax=(CALX(1)+CALX(2))/2;
    int ymin=(CALY(0)+CALY(1))/2;
    int ymax=(CALY(3)+CALY(2))/2;
    
    const int xalpha=25;
    float xb=(320.-25.)/((float)xmax-(float)(xmin));
    int xbeta=xb*512;

    const int yalpha=25;   
    float yb=(240.-25.)/((float)ymax-(float)(ymin));
    int ybeta=yb*512;
   
    while(1)
    {
        xDelay(10);
        switch(mState)
        {
            
            case XP_IDLE:
                 mSem->take();
                 mLast=millis();
                 mState=XP_CAPTURING;
                 mCounter=0;
                 continue;
                 break;
            case XP_CAPTURING:
            {
                int x,y;
                
                if(!rawRead(x,y))
                {
                       interruptsOn();
                       mState=XP_IDLE;
                       continue;
                }
                mCounter++;
                if(mCounter>SINGLE_THRESHOLD)
                {
                    x=xalpha+(((x-xmin)*xbeta)>>9);
                    y=yalpha+(((y-ymin)*ybeta)>>9);
                    if(x<0) continue;
                    if(y<0) continue;

                    if(mHooks)
                    {
                        mHooks->pressEvent(x,y);
                    }       
                    mState=XP_HOLDOFF;
                    mCounter=HOLDOFF;
                }
            }
                break;
            case XP_HOLDOFF:
                mCounter--;
                if(mCounter<=0)
                {
                    interruptsOn();
                    mState=XP_IDLE;
                    continue;
                }
            }

    }
}

// EOF