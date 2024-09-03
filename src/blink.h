#ifndef BLINK_H
#define BLINK_H

#include <Arduino.h>

class blink
{
    private:
        int pin;
        unsigned long interval;
    public:
    blink(int pin);
    void init();
    void update(unsigned long interval);
    void on();
    void off();

}; 

#endif



