#include "mpu6050.h"
#include <Wire.h>
#include <Arduino.h>
#include <math.h>

MPU6050::MPU6050 (bool high_address) :
    current_accel(_2g),
    current_gyro(_250dps)
{
    this->high_address = high_address;
    Wire.begin();
}


bool MPU6050::begin ()
{
    // start normal configuration
    reload();

    gyro_x_offset = 0, gyro_y_offset = 0, gyro_z_offset = 0;
    double accel_x = 0, accel_y = 0, accel_z = 0;
    double value_x, value_y, value_z;

    // calibrate gyro
    for (int i = 0; i < 10; i++)
    {
        readGyro(value_x, value_y, value_z);

        gyro_x_offset = (gyro_x_offset*9 + value_x)/10;
        gyro_y_offset = (gyro_y_offset*9 + value_y)/10;
        gyro_z_offset = (gyro_z_offset*9 + value_z)/10;

        delay(50);
    }

    // calibrating acceleration is not easy, mainly because three diferent
    // positions of the sensor would be needed to obtain the offset of each
    // axis, so I will leave it for now
/*
    // calibrate accel
    for (int i = 0; i < 10; i++)
    {
        readGyro(value_x, value_y, value_z);

        accel_x = (accel_x*9 + value_x)/10;
        accel_y = (accel_y*9 + value_y)/10;
        accel_z = (accel_z*9 + value_z)/10;

        delay(50);
    }

    // solve delta_a for the equation
    // 9.81^2 = (accel_x + delta_a)^2 + (accel_y + delta_a)^2 + (accel_x + delta_a)^2
    double b = 2*(accel_x + accel_y + accel_z);
    double c = accel_x*accel_x + accel_y*accel_y + accel_z*accel_z* - 9.81*9.81;
    accel_offset = (-b + sqrt(b*b - 12*c))/6;

    Serial.println(accel_offset);

    // check for negative sqrt errors
    if (accel_offset == NAN)
        accel_offset = 0;
        */
    
    accel_offset = 0;
}


bool MPU6050::reload ()
{
    mpu_failed = false;

    // stop sleeping
    setSleep(false);

    // write accel and gyro max reading values
    setGyroDPS(current_gyro);
    setAccelMps(current_accel);
}


void MPU6050::setSleep (bool sleep)
{
    // write to the sleep register
    write(0x6B, sleep<<6);
}


void MPU6050::write (byte address, byte data)
{
    bool result;
    Wire.beginTransmission(MPU_ADDRESS + high_address);
    Wire.write(address);
    Wire.write(data);
    result = Wire.endTransmission();

    // if MPU has failed in the bast, don't overwrite
    if (not mpu_failed)
        mpu_failed = result;
}


void MPU6050::request (byte address, unsigned n_bits)
{
    bool result;
    Wire.beginTransmission(MPU_ADDRESS + high_address);
    Wire.write(address);
    result = Wire.endTransmission();
    Wire.requestFrom(MPU_ADDRESS + high_address,n_bits);

    // if MPU has failed in the bast, don't overwrite
    if (not mpu_failed)
        mpu_failed = result;
}


void MPU6050::setFilter (MPU6050_filter filter)
{
    // change signal filtering
    write(0x1A, filter);
}


void MPU6050::setGyroDPS (MPU6050_gyro dps)
{
    // change max degrees per second of the gyro
    write(0x1B, dps << 3);
}


void MPU6050::setAccelMps (MPU6050_accel mps2)
{
    // change max acceleration reading of the accelerometer
    write(0x1C, mps2 << 3);
}


void MPU6050::readAccel (double &acc_x, double &acc_y, double &acc_z)
{
    int16_t AccX, AccY, AccZ;

    request(0x3B, 6);

    AccX = Wire.read()<<8 | Wire.read();
    AccY = Wire.read()<<8 | Wire.read();
    AccZ = Wire.read()<<8 | Wire.read();

    // conversion from base 16 reading to m/s2
    double correction;

    switch (current_accel)
    {
        case _2g:
            correction = 16384;
            break;
        case _4g:
            correction = 8192;
            break;
        case _8g:
            correction = 4096;
            break;
        case _16g:
            correction = 2048;
            break;
    }

    acc_x = AccX/correction*9.81 - accel_offset;
    acc_y = AccY/correction*9.81 - accel_offset;
    acc_z = AccZ/correction*9.81 - accel_offset;
}


double MPU6050::readTemp ()
{
    int16_t t_raw;

    request(0x41, 2);

    t_raw = Wire.read()<<8 | Wire.read();

    return t_raw/340.f + 36.53;
}


void MPU6050::readGyro (double &gyro_x, double &gyro_y, double &gyro_z)
{
    int16_t raw_x, raw_y, raw_z;

    request(0x43, 6);

    raw_x = Wire.read()<<8 | Wire.read();
    raw_y = Wire.read()<<8 | Wire.read();
    raw_z = Wire.read()<<8 | Wire.read();

    double correction;

    switch (current_gyro)
    {
        case _250dps:
            correction = 131;
            break;
        case _500dps:
            correction = 65.5;
            break;
        case _1000dps:
            correction = 32.8;
            break;
        case _2000dps:
            correction = 16.4;
            break;
    }

    gyro_x = raw_x/correction - gyro_x_offset;
    gyro_y = raw_y/correction - gyro_y_offset;
    gyro_z = raw_z/correction - gyro_z_offset;
}


bool MPU6050::failed () const
{
    return mpu_failed;
}
