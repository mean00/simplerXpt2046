
#pragma once
#include "SPI.h"
#include "MapleFreeRTOS1000_pp.h"


class XPT2046Hook
{
public:
    virtual void pressEvent(int x,int y)=0;
};

/**
 * 
 */
class XPT2046
{
public:    
            virtual          ~XPT2046() {}
            virtual bool     setup(int *calibrationData)=0;
            virtual void     start()=0;            
            virtual void     setHooks(XPT2046Hook *h) =0;
                    
            virtual bool     rawRead(int &x, int &y)=0; // This is only used for calibration
            
    static XPT2046 *spawn(SPIClass &spi, int cs,int irq,int speed,xMutex *tex);
protected:
        XPT2046()
        {
            
        };
};

// EOF