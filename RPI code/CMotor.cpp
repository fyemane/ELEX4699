#include <algorithm>
#include <chrono>
#include <thread>
#include "cvui.h"
#include "pigpio.h"

#include "Constants.h"
#include "CControlPi.h"
#include "CMotor.h"


CMotor::CMotor(int motor_pin) : _pinPWM(motor_pin) {
    if (gpioInitialise() < 0) {
        std::cerr << "GPIO initialization failed" << std::endl;
    }
    gpioSetMode(_pinPWM, PI_OUTPUT);

    _speed = 0.5;
    _duration = 1;
}

CMotor::~CMotor() {
    gpioTerminate();
}

void CMotor::go() {
    gpioPWM(_pinPWM, _speed * 255);

//    gpioServo(_leftPinPWM, 1100);
//    gpioServo(_rightPinPWM, 1900);
    if (_duration != 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(_duration)));
        stop();
    }
}


void CMotor::stop() {
    _speed = 0.5;
    _duration = 0;
}
