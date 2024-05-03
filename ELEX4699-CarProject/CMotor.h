#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>

#include "pigpio.h"
#include "Constants.h"
#include "CControlPi.h"

class CMotor
{
private:
    int _pinPWM;
    float _speed;

public:
    CMotor(int motor_pin, int speed);
    ~CMotor();
    void go();
    void stop();
    void shoot();
    void shoot_stop();

    // getters
    inline int get_pinPWM() {return _pinPWM;}
    inline float get_speed() {return _speed;}

    // setters
    void set_pinPWM(int pinPWM) {_pinPWM = pinPWM;}
    void set_speed(float speed) {_speed = speed;}

};

