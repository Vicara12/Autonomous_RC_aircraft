#ifndef HCSR04_h_
#define HCSR04_h_


class HCSR04
{
public:

    HCSR04 (int trigger_pin, int echo_pin);

    // returns -1 if timeout is reached
    double measure ();

    // t in degrees celsius
    void setT (double t);

    void setMaxWait (unsigned long max_wait);

private:

    // returns the speed of sound for a given temperature
    static double getSoundSpeed (double t);


    int trigger_pin, echo_pin;

    double sound_speed;
    unsigned long max_wait;
};


#endif