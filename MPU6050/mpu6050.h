#ifndef MPU6050_h_
#define MPU6050_h_

#define MPU_ADDRESS 0x68

typedef unsigned char byte;

enum MPU6050_filter {_256Hz, _188Hz, _98Hz, _42Hz, _20Hz, _10Hz, _5Hz};
enum MPU6050_gyro {_250dps, _500dps, _1000dps, _2000dps};
enum MPU6050_accel {_2g, _4g, _8g, _16g};


/*

If the power to the sensor fails and it restarts, readings could be wrong
(in case max dg/s or acceleration aren't 250dg/s and 2g respectively and the
chip had a power down or reset of any kind).
To avoid that the following use is recomended:


void setup(){
    mpu.begin();
}

void loop(){

    if (mpu.failed())
        mpu.reload();

    // reading
}

*/

class MPU6050
{
public:

    MPU6050 (bool high_address);

    // this function must be called with the sensor still
    // this method must be called before reload at least once
    bool begin ();

    // this function can be called at any time, without the sensor needing
    // to be still
    bool reload ();

    // put the sensor in sleep mode
    void setSleep (bool sleep);

    // change digital low pass filter frequency
    void setFilter (MPU6050_filter filter);

    // change max degrees per second of the gyro
    void setGyroDPS (MPU6050_gyro dps);

    // change max m/s2 reading of the sensor
    void setAccelMps (MPU6050_accel mps);

    // values in m/s2
    void readAccel (double &acc_x, double &acc_y, double &acc_z);

    // results in degrees
    double readTemp ();

    // results in gd/s
    void readGyro (double &gyro_x, double &gyro_y, double &gyro_z);

    // returns wether there was a comunication error with the chip
    bool failed () const;

private:

    // wrtie data to MPU6050 selected address, returns true if successful
    void write (byte address, byte data);

    // request n_bits to MPU6050 from selected address, returns true if successful
    void request (byte address, unsigned n_bits);

    bool mpu_failed;
    bool high_address;

    double gyro_x_offset, gyro_y_offset, gyro_z_offset;
    double accel_offset;

    MPU6050_accel current_accel;
    MPU6050_gyro current_gyro;
};


#endif
