#include "bmp280.h"
#include <Wire.h>
#include <math.h>
#include <Arduino.h>


BMP280::BMP280 (bool high_address) :
    current_p_oversampling(x16),
    current_filtering(0)
{
    this->high_address = high_address;
    Wire.begin();
}


void BMP280::begin ()
{
    readCalibrationConstants();
    reload();
}


void BMP280::reload ()
{
    bmp_failed = false;

    sleep(false);
    setFilter(current_filtering);
    setPOversampling(current_p_oversampling);
}


void BMP280::write (byte address, byte data)
{
    bool result;
    Wire.beginTransmission(BMP_ADDRESS + high_address);
    Wire.write(address);
    Wire.write(data);
    result = Wire.endTransmission();

    // if MPU has failed in the bast, don't overwrite
    if (not bmp_failed)
        bmp_failed = result;
}


void BMP280::request (byte address, unsigned n_bits)
{
    bool result;
    Wire.beginTransmission(BMP_ADDRESS + high_address);
    Wire.write(address);
    result = Wire.endTransmission();
    Wire.requestFrom(BMP_ADDRESS + high_address,n_bits);

    // if MPU has failed in the bast, don't overwrite
    if (not bmp_failed)
        bmp_failed = result;
}


void BMP280::setPOversampling (POversampling oversampling)
{
    current_p_oversampling = oversampling;

    request(0xF4, 1);

    byte value = Wire.read() & 0x03; // read first two bytes of the register
    // set new values for the t and p oversampling
    value |= oversampling<<2 | (oversampling == x16 ? x1<<5 : x2<<5);

    write(0xF4, value);
}


void BMP280::sleep (bool sleep)
{
    request(0xF4, 1);

    byte value = Wire.read() & 0xFC; // store all bytes except last two
    // set new values for the t and p oversampling
    value |= (sleep ? 0x00 : 0x03);

    write(0xF4, value);
}


void BMP280::setFilter (byte filter_coeff)
{
    write(0xF5, (filter_coeff&0x03)<<2);
}


bool BMP280::failed () const
{
    return bmp_failed;
}


void BMP280::readCalibrationConstants ()
{
    request(0x88, 0x9F-0x88+1);

    dig_T1 = Wire.read() | Wire.read()<<8;
    dig_T2 = Wire.read() | Wire.read()<<8;
    dig_T3 = Wire.read() | Wire.read()<<8;
    dig_P1 = Wire.read() | Wire.read()<<8;
    dig_P2 = Wire.read() | Wire.read()<<8;
    dig_P3 = Wire.read() | Wire.read()<<8;
    dig_P4 = Wire.read() | Wire.read()<<8;
    dig_P5 = Wire.read() | Wire.read()<<8;
    dig_P6 = Wire.read() | Wire.read()<<8;
    dig_P7 = Wire.read() | Wire.read()<<8;
    dig_P8 = Wire.read() | Wire.read()<<8;
    dig_P9 = Wire.read() | Wire.read()<<8;
}


void BMP280::read (double &p, double &t)
{
    long adc_T, adc_P;

    request(0xF7, 0xFC-0xF7+1);

    // read raw p and t values
    adc_P = ((long)Wire.read()&0xFF)<<12 | ((long)Wire.read()&0xFF)<<4 | ((long)Wire.read()&0xFF)>>4;
    adc_T = ((long)Wire.read()&0xFF)<<12 | ((long)Wire.read()&0xFF)<<4 | ((long)Wire.read()&0xFF)>>4;

    double var1, var2, t_fine;

    // get correct temperature
    var1  = (((double)adc_T)/16384.0 - ((double)dig_T1)/1024.0)*((double)dig_T2);
    var2  = ((((double)adc_T)/131072.0 - ((double)dig_T1)/8192.0)*(((double)adc_T)/131072.0 - ((double)dig_T1)/8192.0))*((double)dig_T3);
    t_fine = (long)(var1 + var2);
    t  = (var1 + var2) /5120.0;

    // get correct pressure
    var1 = ((double)t_fine/2.0) - 64000.0;
    var2 = var1*var1*((double)dig_P6)/32768.0;
    var2 = var2 + var1*((double)dig_P5)*2.0;
    var2 = (var2/4.0)+(((double)dig_P4)*65536.0);
    var1 = (((double)dig_P3)*var1*var1/524288.0 + ((double)dig_P2)*var1)/524288.0;
    var1 = (1.0 + var1/32768.0)*((double)dig_P1);

    if(var1 == 0.0)
    {
        p = 0;
        return;
    }

    p = 1048576.0 - (double)adc_P;
    p = (p - (var2/4096.0))*6250.0/var1;
    var1 = ((double)dig_P9)*p*p/2147483648.0;
    var2 = p*((double)dig_P8)/32768.0;
    p = p + (var1 + var2 + ((double)dig_P7))/16.0;
}


double BMP280::getHeight (double p, double reference_p)
{
    return 44330.0 * (1.0 - pow(p / reference_p, 0.19026));
}


double BMP280::getSpeed (double dynamic_p, double static_p)
{
    // air density: rho = 1.225 kg/m^3 at sea level
    return sqrt(2*(dynamic_p-static_p)/1.225);
}


void BMP280::readAverage (double &p, double &t, unsigned n)
{
    double measured_p, measured_t;
    
    read(p, t);

    for (int i = 0; i < n-1; i++)
    {
        read(measured_p, measured_t);

        p = (p*(n-1) + measured_p)/n;
        t = (t*(n-1) + measured_t)/n;

        delay(10);
    }
}