#include "pid.h"

PID::PID (float k_p, float k_d, float k_i) :
    prev_error(0),
    prev_time(-1),
    kp(k_p),
    kd(k_d),
    ki(k_i),
    error_integral(0)
{
    //
}


void PID::setKc (float k_p) {kp = k_p;}


void PID::setKd (float k_d) {kd = k_d;}


void PID::setKi (float k_i) {ki = k_i;}


float PID::actualize (float error, float t)
{
    float delta_t = prev_time > 0 ? t - prev_time : 0;

    error_integral += error*delta_t;

    float error_derivative = delta_t != 0 ? (error - prev_error)/delta_t : 0;

    float result = kp*error + ki*error_integral + kd*error_derivative;

    prev_error = error;
    prev_time = t;

    return result;
}