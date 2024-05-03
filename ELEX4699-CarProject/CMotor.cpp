#include "CMotor.h"

CMotor::CMotor(int motor_pin, int speed) : _pinPWM(motor_pin) {
    if (gpioInitialise() < 0) {
        std::cout << "GPIO initialization failed" << std::endl;
    }
    gpioSetMode(_pinPWM, PI_OUTPUT);

    _speed = speed;
}

CMotor::~CMotor() {
    gpioTerminate();
}

void CMotor::go() {
    gpioServo(_pinPWM, _speed);
}

void CMotor::stop() {
    _speed = 1500;
}
