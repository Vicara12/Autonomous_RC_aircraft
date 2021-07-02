#include "hcsr04.h"
#include <math.h>
#include <Arduino.h>


HCSR04::HCSR04 (int trigger_pin, int echo_pin)
{
    this->trigger_pin = trigger_pin;
    this->echo_pin = echo_pin;
    sound_speed = getSoundSpeed(20);
    max_wait = 10e3; // 10 milliseconds (aprox 2 meters)

    pinMode(trigger_pin, OUTPUT);
    pinMode(echo_pin, INPUT);

    digitalWrite(trigger_pin, LOW);
}


double HCSR04::measure ()
{
    digitalWrite(trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger_pin, LOW);

    unsigned long duration = pulseIn(echo_pin, HIGH, max_wait);

    // if tiemeout reached, echo has been lost
    if (duration == 0)
        return -1;
    
    return sound_speed*duration/2e6;
}


void HCSR04::setT (double t)
{
    sound_speed = getSoundSpeed(t);
}


void HCSR04::setMaxWait (unsigned long max_wait)
{
    this->max_wait = max_wait;
}


double HCSR04::getSoundSpeed (double t)
{
    return sqrt(1.4*286*(273.15+t));
}
