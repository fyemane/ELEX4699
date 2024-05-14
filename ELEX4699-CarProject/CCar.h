#pragma once
#include <algorithm>
#include <chrono>
#include <thread>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <chrono>

#include "server.h"
#include "Client.h"
#include "Constants.h"
#include "CMotor.h"
#include "CControlPi.h"

class CCar {

private:
    CMotor _lmotor{LMOTOR_OUTPUT_PIN, 1500};
    CMotor _rmotor{RMOTOR_OUTPUT_PIN, 1500};

    CControlPi control_obj;

    CClient _pi_client;
    CServer _pi_server;

    std::string _arena_server_ip;
    std::string _pi_server_ip;

    int _pi_server_port;
    int _arena_server_port;
    int _shoot_speed;

    std::chrono::steady_clock::time_point pulse_start_time;
    std::chrono::steady_clock::time_point pulse_end_time;
    std::chrono::microseconds pulse_duration;

    std::chrono::steady_clock::time_point func_start_time;
    std::chrono::steady_clock::time_point func_end_time;
    std::chrono::milliseconds func_duration;
    double distance;
    double prev_distance;
    int forward_stop_offset;

    double _analogX, _analogY;
    std::string cmd1;
    double cmd2, cmd3, cmd4, cmd5, cmd6;

    cv::Size _canvasSize;
    cv::VideoCapture _vid;
    cv::Mat _canvas;
    cv::Mat _vidFrame;
    cv::Mat _processedFrame;
    cv::Mat _persistentCanvas; ///< Mat to store persistent drawing.
    cv::Point _gui_position; ///< Position for graphical user interface.
    int _gui_vertspace;

    std::vector<std::string> _commands_list;
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
    std::vector<double> areas;
    std::vector<cv::Point2f> centers;

    int _target_aruco_id;
    double _target_aruco_area;
    cv::Point _target_aruco_center;


    bool _autonomous;
    bool _thread_exit;
    bool _resetFlag, _exitFlag; ///< Flags for resetting and exiting the application.

    int _pwm_lmotor_forward, _pwm_rmotor_forward;
    int _pwm_lmotor_backward, _pwm_rmotor_backward;
    int _pwm_lmotor_left, _pwm_rmotor_left;
    int _pwm_lmotor_right, _pwm_rmotor_right;
    int _pwm_lmotor_stop, _pwm_rmotor_stop;
    int _pwm_lmotor_CW , _pwm_rmotor_CW;


public:
    CCar(int mode);
    ~CCar();

    void update();
    void draw();
    void drawImage(cv::Mat& canvas, cv::Point position, cv::Mat& image);
    void autonomous();

    // drive functions
    void lmotor_drive();
    void rmotor_drive();
    void shoot();
    void shoot_stop();
    void sense_distance();
    void update_target_aruco();

    void get_commands();

    // motor drive threads
    static void update_target_aruco_thread(CCar* ptr);
    static void autonomous_thread(CCar* ptr);
    static void lmotor_drive_thread(CCar* ptr);
    static void rmotor_drive_thread(CCar* ptr);
    static void get_commands_thread(CCar* ptr);
    static void draw_thread(CCar* ptr);
    static void update_thread(CCar* ptr);
    static void ultrasonic_sensor_thread(CCar* ptr);

    void run();


};
