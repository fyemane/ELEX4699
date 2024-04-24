#include "pigpio.h"
#include <thread>
#include <chrono>

class CMotor
{
private:
    int _pinPWM;
    float _speed;
    float _duration;
    bool _duration_done;

public:
    CMotor(int motor_pin);
    ~CMotor();
    void go();
    void stop();

    // getters
    inline int get_pinPWM() {return _pinPWM;}
    inline float get_speed() {return _speed;}
    inline float get_duration() {return _duration;}
    // setters
    void set_pinPWM(int pinPWM) {_pinPWM = pinPWM;}
    void set_speed(float speed) {_speed = speed;}
    void set_duration(float duration) {_duration = duration;}

};

