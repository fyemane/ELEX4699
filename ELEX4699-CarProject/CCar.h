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

    int _shoot_speed;

    double _analogX, _analogY;
    std::string cmd1;
    double cmd2, cmd3, cmd4, cmd5, cmd6, cmd7;

    cv::Size _canvasSize;
    cv::VideoCapture _vid;
    cv::Mat _canvas;
    cv::Mat _vidFrame;
    cv::Mat _processedFrame;
    cv::Mat _persistentCanvas; ///< Mat to store persistent drawing.
    cv::Point gui_position; ///< Position for graphical user interface.


    bool _thread_exit;
    bool _resetFlag, _exitFlag; ///< Flags for resetting and exiting the application.


public:
    CCar();
    ~CCar();

    void update();
    void draw();
    void drawImage(cv::Mat& canvas, cv::Point position, cv::Mat& image);

    // drive functions
    void lmotor_drive();
    void rmotor_drive();
    void shoot();
    void shoot_stop();

    void get_commands();

    // motor drive threads
    static void lmotor_drive_thread(CCar* ptr);
    static void rmotor_drive_thread(CCar* ptr);
    static void get_commands_thread(CCar* ptr);
    static void draw_thread(CCar* ptr);
    static void update_thread(CCar* ptr);


    void run();


};
