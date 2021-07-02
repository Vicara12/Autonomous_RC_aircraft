#ifndef BMP280_h_
#define BMP280_h_


#define BMP_ADDRESS 0x76

typedef unsigned char byte;

enum POversampling {x0, x1, x2, x4, x8, x16};

class BMP280
{
public:

    BMP280 (bool high_address);

    void begin ();

    // call this method if a comunication error is detected
    void reload ();

    // returns wether there was a comunication error with the chip
    bool failed () const;

    // put device to sleep or set it to normal
    void sleep (bool sleep);

    // filter_coeff must be between 0 and 7, both included
    void setFilter (byte filter_coeff);

    // set pressure oversampling (tamperature oversampling will be set accordingly)
    void setPOversampling (POversampling oversampling);

    // get pressure and temperature from the sensor in Pa and ºC, respectively
    void read (double &p, double &t);

    // reads t and p n times and returns the average
    void readAverage (double &p, double &t, unsigned n);

    // calculates height to reference_p in meters
    double getHeight (double p, double reference_p);

    // returns speed in m/s given a static pressure
    // dynamic p is the pressure measured in a tube facing directly into the wind
    // static p is the pressure measured perpendicular to the wind
    double getSpeed (double dynamic_p, double static_p);

private:

    // wrtie data to BMP280 selected address, returns true if successful
    void write (byte address, byte data);

    // request n_bits to BMP280 from selected address, returns true if successful
    void request (byte address, unsigned n_bits);

    void readCalibrationConstants ();


    bool bmp_failed;
    bool high_address;

    POversampling current_p_oversampling;
    byte current_filtering;

    unsigned dig_T1, dig_P1;
    int dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5;
    int dig_P6, dig_P7, dig_P8, dig_P9;
};


#endif