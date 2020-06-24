#include "mbed.h"
#include "InterruptIn.h"
#include "SoftPWM.h"

SoftPWM::SoftPWM(PinName _outpin): pulse(_outpin)    //constructa  
{
    pulse = 0;
    interval = 0.02;
    width = 0;
//    start();
}

float SoftPWM::read()
{
    if (width <= 0.0)return 0.0;
    if (width > 1.0) return 1.0;
    return width / interval;    
}

void SoftPWM::write(float duty)
{
    width = interval * duty;
    if (duty <= 0.0)width =  0.0;
    if (duty > 1.0) width =  interval;
}

void SoftPWM::start()
{
    if (width != 0.0) {
        pulse = 1;
       _ticker.attach(callback(this, &SoftPWM::TickerInterrapt), interval);
    } else {
        pulse = 0;
    }
}

void SoftPWM::stop()
{
    pulse = 0;
    _ticker.detach();
}

void SoftPWM::period(float _period)
{
    interval = _period;
    start();
}

void SoftPWM::period_ms(int _period)
{
    period((float)_period / 1000);
    start();
}

void SoftPWM::period_us(int _period)
{
    period((float)_period / 1000000);
    start();
}

void SoftPWM::pulsewidth(float _width)
{
    width = _width;
   if (width < 0.0)width = 0.0;
}

void SoftPWM::pulsewidth_ms(int _width)
{
     pulsewidth((float)_width / 1000);
}

void SoftPWM::pulsewidth_us(int _width)
{
    pulsewidth((float)_width / 1000000);
}

void SoftPWM::TickerInterrapt()
{ 
    if (width <= 0)return;
    _timeout.attach(callback(this, &SoftPWM::end), width);
    pulse = 1; 
}

void SoftPWM::end()
{
    pulse = 0;
//    _timeout.detach();
}
;

