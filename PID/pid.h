#ifndef PID_H_
#define PID_H_


class PID
{
public:

    PID (float k_p, float k_d, float k_i);
    
    void setKc (float k_p);
    void setKd (float k_d);
    void setKi (float k_i);
    
    float actualize (float error, float t);

private:

    float kp, kd, ki;
    float error_integral;
    float prev_time;
    float prev_error;
};


#endif