#pragma once
#include <algorithm>
#include <chrono>
#include <thread>
#include <opencv2/opencv.hpp>
#include <sstream>

#include "server.h"
#include "Client.h"
#include "Constants.h"
#include "CMotor.h"

class CCar
{
private:
    CMotor _lmotor{LMOTOR_OUTPUT_PIN, 1500};
    CMotor _rmotor{RMOTOR_OUTPUT_PIN, 1500};

    CClient _pi_client;
    CServer _pi_server;

    std::string _arena_server_ip;
    std::string _pi_server_ip;

    int _pi_server_port;
    int _arena_server_port;

    std::vector<std::string> _commands_list;

    double _analogX, _analogY;
    std::string cmd1;
    double cmd2, cmd3, cmd4, cmd5, cmd6;

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
    static void serverstart_thread(CCar* ptr);


    void run();


};
