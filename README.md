# Simpler XPT2046

This is a simple XPT2046 touchscreen helper class
It assumes you are using the c++ wrapper of freeRTOS & ArduinoStm32

The main features are :
* It is task + interrupt based
* The time spent under interrupt is very very small
* It uses a mutex to protect the SPI from concurrent accesses
* It is callback based, i.e. you get called with the coordinates of the pressed point (warning you are called from within the XPT2046 task!)
* It is relatively simple


Dont forget to do the calibration
Careful the code is hardcoded for a 320x240 screen 
