#ifndef _PUMP
#define _PUMP
#include <Arduino.h>

class Pump
{
private:
    int _pin;
    bool _state;

public:
    Pump(uint8_t pin)
    {
        _pin = pin;
        _state = false;
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, _state);
    }
    void toggle()
    {
        _state = !_state;
        digitalWrite(_pin, _state);
    }
};

#endif