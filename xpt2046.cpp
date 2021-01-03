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

bool xpIrq=false;
/**
 * 
 * @param a
 */
void XPT2046impl::irqAnon(void *a)
{
    XPT2046impl *x=(XPT2046impl *)a;
    x->irq();
}
/**
 * 
 */
void XPT2046impl::irq()
{
    mSem->giveFromInterrupt();
    interruptsOff();
}
/**
 * 
 */
void    XPT2046impl::interruptsOn()
{
    
    attachInterrupt(mIrq,XPT2046impl::irqAnon,this,FALLING);
    xpIrq=true;
}
/**
 * 
 */
void    XPT2046impl::interruptsOff()
{
    detachInterrupt(mIrq);
    xpIrq=false;
}

/**
 * 
 */
XPT2046impl::XPT2046impl(SPIClass &spi, int cs,int irq,int speed,xMutex *x) : mSPI(spi),mSettings(speed,MSBFIRST, SPI_MODE0), xTask("TOUCH",3,200)
{
    mCs=cs;
    mIrq=irq;
    mFrequency=speed;
    
    interruptsOff();    
    mState=XP_IDLE;
    pinMode(mIrq, INPUT_PULLUP);
    pinMode(mCs,OUTPUT);
    digitalWrite(mCs,HIGH);
    
    mSem=new xBinarySemaphore;
    mTex=x;
    cap=0;
    mCalibration=NULL;
    vTaskSuspend(_taskHandle);
    mHooks=NULL;
   
    
}
/**
 * 
 */
 void    XPT2046impl::start()
 {
     interruptsOn();
     vTaskResume(_taskHandle);
 }

/**
 * 
 */
XPT2046impl::~XPT2046impl()
{
    
}




/**
 * 
 * @return 
 */
bool     XPT2046impl::setup(int *calibrationData)
{
    mCalibration=calibrationData;
    return true;
}

static void pt(int r, int value)
{
    char bfer[20];
    sprintf(bfer,"%d: 0x%x",r,value);
    Serial1.println(bfer);
}
/**
 * 
 * @param spi
 * @param cs
 * @param irq
 * @param speed
 * @param tex
 * @return 
 */
 XPT2046 *XPT2046::spawn(SPIClass &spi, int cs,int irq,int speed,xMutex *tex)
 {
     return new XPT2046impl(spi,cs,irq,speed,tex);
 }

// EOF