
#pragma once
#include "xpt2046.h"

/**
 * 
 */
class XPT2046impl: public XPT2046, xTask
{
public:    
                     XPT2046impl(SPIClass &spi, int cs,int irq,int speed,xMutex *tex);
            virtual  ~XPT2046impl();
            bool     setup(int *calibrationData);
            void     run();
            void     start();
            bool     rawRead(int &x, int &y);
            void     setHooks(XPT2046Hook *h) 
                    {
                        mHooks=h;
                    }
  static    void    irqAnon(void *a);            
            void    irq();
protected:
        bool        mIdle;
        SPIClass    &mSPI;    
        int         mCs;
        int         mFrequency;
        SPISettings mSettings;
        int         mRawData[20];
        int         median(int a, int b, int c,int d);
        xMutex      *mTex;
        int         cap;
        int         *mCalibration;
        int         mIrq;        
        XPT2046Hook *mHooks;
        xBinarySemaphore *mSem;
        void    interruptsOn();
        void    interruptsOff();
};

// EOF