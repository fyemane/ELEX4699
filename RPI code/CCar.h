#pragma once
#include <algorithm>
#include <chrono>
#include <thread>
#include <opencv2/opencv.hpp>

#include "server.h"
#include "Constants.h"
#include "CMotor.h"

class CCar
{
private:
    CMotor _lmotor{LMOTOR_OUTPUT_PIN};
    CMotor _rmotor{RMOTOR_OUTPUT_PIN};
    bool _thread_exit;

public:
    CCar();
    ~CCar();

    void update();

    // drive functions
    void lmotor_drive();
    void rmotor_drive();

    // motor drive threads
    static void lmotor_drive_thread(CCar* ptr);
    static void rmotor_drive_thread(CCar* ptr);

    void run();


};
