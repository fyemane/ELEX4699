#include "CCar.h"
#include "cvui.h"

CCar::CCar(int mode) {
    _pi_server_port = 4012;

    _thread_exit = false;
    _resetFlag = false;
    _exitFlag = false;

    _shoot_speed = 500;
    _autonomous = mode;
    forward_stop_offset = 2000;


    _pwm_lmotor_forward = 1560;
    _pwm_rmotor_forward = 500;

    _pwm_lmotor_backward = 1405;
    _pwm_rmotor_backward = 2500;

    _pwm_lmotor_left = 1490;
    _pwm_rmotor_left = 500;

    _pwm_lmotor_right = 2500;
    _pwm_rmotor_right = 1500;

    _pwm_lmotor_stop = 1490;
    _pwm_rmotor_stop = 1500;

    _pwm_lmotor_CW = 2500;
    _pwm_rmotor_CW = 2500;



    // for drawing
    _canvasSize = cv::Size(1200, 700);
    _canvas = cv::Mat::zeros(_canvasSize, CV_8UC3);
    _persistentCanvas = cv::Mat::zeros(_canvasSize, CV_8UC3);  // Initialize persistent canvas
    cvui::init("CarProject");              // Initialize cvui here
    _gui_position = cv::Point(650, 10);
    _gui_vertspace = 50;


    _vid.open(0, cv::CAP_V4L2);
    if (!_vid.isOpened()) {
        std::cerr << "Error: Unable to open camera" << std::endl;
    }

    if (gpioInitialise() < 0) {
        std::cerr << "GPIO initialization failed" << std::endl;
    }

    // Create window for displaying canvas
    cv::namedWindow("CarProject", cv::WINDOW_AUTOSIZE);

    // robot should not move when the object is created
    _lmotor.set_speed(1490);
    _rmotor.set_speed(1500);

    // initialize the ultrasonic sensor
    control_obj.set_digital(TRIGGER_PIN, 0);
    std::this_thread::sleep_for(std::chrono::seconds(2));

}

CCar::~CCar() {
    gpioTerminate();
    _vid.release();
}

void CCar::update() {

    // rows is 480, cols is 640
    _vid >> _vidFrame;
    _vid >> _processedFrame;

    if (_vidFrame.empty() == false) {
        _pi_server.set_txim(_vidFrame);
        cv::aruco::detectMarkers(_vidFrame, dictionary, corners, ids);

        areas.clear();
        centers.clear();

        for (size_t i = 0; i < corners.size(); i++) {
            double area = cv::contourArea(corners[i]);
            cv::Moments M = cv::moments(corners[i]);
            cv::Point center(M.m10 / M.m00, M.m01 / M.m00);

            areas.push_back(area);
            centers.push_back(center);
        }

        if (ids.size() > 0) {
            cv::aruco::drawDetectedMarkers(_processedFrame, corners, ids);
        }
    }

    _pi_server.get_cmd(_commands_list);

    if (!_commands_list.empty()) {
        for (const auto& str: _commands_list) {
            //std::cout << "Received: " << str << std::endl;
        }
        std::istringstream iss(_commands_list.at(0));

        // parse string
        iss >> cmd1 >> cmd2 >> cmd3 >> cmd4 >> cmd5 >> cmd6;

        if (!_autonomous) {
                if(cmd1 != "G"){
                // drive forward
                if (cmd3 > 12000) {
                    // only drive if no obstruction in front of ultrasonic sensor
                    if (distance > forward_stop_offset) {
                        _lmotor.set_speed(_pwm_lmotor_forward);
                        _rmotor.set_speed(_pwm_rmotor_forward);
                    }
                    // stop motor throttle with distance sensing
                    if (distance < forward_stop_offset) {
                        _lmotor.set_speed(_pwm_lmotor_stop);
                        _rmotor.set_speed(_pwm_rmotor_stop);
                    }
                }
                //drive backwards
                else if(cmd3 < -12000) {
                    _lmotor.set_speed(_pwm_lmotor_backward);
                    _rmotor.set_speed(_pwm_rmotor_backward);
                }
                // stop driving
                else {
                    _lmotor.set_speed(_pwm_lmotor_stop);
                    _rmotor.set_speed(_pwm_rmotor_stop);
                }

                // turn left
                if (cmd4 < -12000) {
                    _lmotor.set_speed(_pwm_lmotor_left);
                    _rmotor.set_speed(_pwm_rmotor_left);
                }
                //turn right
                else if (cmd4 > 12000) {
                    _lmotor.set_speed(_pwm_lmotor_right);
                    _rmotor.set_speed(_pwm_rmotor_right);
                }

                // rotate CW
                if (cmd6 > 0) {
                    _lmotor.set_speed(_pwm_lmotor_CW);
                    _rmotor.set_speed(_pwm_rmotor_CW);
                }

                // shoot
                if (cmd5 > 0) {
                    shoot();
                }
                else {
                    shoot_stop();
                }
            }
        }

    }

}

void CCar::autonomous() {
    if (_autonomous) {

        _target_aruco_id = 30;
        // move forward
        _lmotor.set_speed(_pwm_lmotor_forward);
        _rmotor.set_speed(_pwm_rmotor_forward);
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
//        // pause for 400 ms
        _lmotor.set_speed(_pwm_lmotor_stop);
        _rmotor.set_speed(_pwm_rmotor_stop);
        std::this_thread::sleep_for(std::chrono::milliseconds(400));

        //while (std::abs(distance) > 1 && _target_aruco_area < 1500) {
        while (distance > forward_stop_offset) {
            // if aruco target is to the left of the pi cam, move left
            if (_target_aruco_center.x < 280) {
                while (_target_aruco_center.x < 280) {
                    _lmotor.set_speed(_pwm_lmotor_stop);
                    _rmotor.set_speed(1480); // decrese this
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    // pause for 200 ms
                    _lmotor.set_speed(_pwm_lmotor_stop);
                    _rmotor.set_speed(_pwm_rmotor_stop);
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));

                    if (distance < forward_stop_offset){
                        break;
                    }

                }
            // if aruco target is the the right of the pi cam, move right
            }
            else if (_target_aruco_center.x > 280) {
                while (_target_aruco_center.x > 280) {
                    _lmotor.set_speed(1505); // increase this
                    _rmotor.set_speed(_pwm_rmotor_stop);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    // pause for 200 ms
                    _lmotor.set_speed(_pwm_lmotor_stop);
                    _rmotor.set_speed(_pwm_rmotor_stop);
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));

                    if (distance < forward_stop_offset){
                        break;
                    }

                }
            }

            _lmotor.set_speed(_pwm_lmotor_stop);
            _rmotor.set_speed(_pwm_rmotor_stop);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            _lmotor.set_speed(_pwm_lmotor_forward);
            _rmotor.set_speed(_pwm_rmotor_forward);
//            if (distance < forward_stop_offset) {
//                _lmotor.set_speed(_pwm_lmotor_stop);
//                _rmotor.set_speed(_pwm_rmotor_stop);
//            }
            std::this_thread::sleep_for(std::chrono::milliseconds(300));

//            // shooting for D1
//            if (_target_aruco_area > 350 && _target_aruco_area < 570)
//                shoot();
//            else
//                shoot_stop();

        }

       _target_aruco_id = 31;

        // make 90 degree angle at arena corner
        _lmotor.set_speed(_pwm_lmotor_right);
        _rmotor.set_speed(_pwm_rmotor_right);
        std::this_thread::sleep_for(std::chrono::milliseconds(600));

        // pause for 200 ms
        _lmotor.set_speed(_pwm_lmotor_stop);
        _rmotor.set_speed(_pwm_rmotor_stop);
        std::this_thread::sleep_for(std::chrono::milliseconds(400));

        // course correction at corner before moving straight
        // if aruco target is to the left of the pi cam, move left
            if (_target_aruco_center.x < 290) {
                while (_target_aruco_center.x < 290) {
                    _lmotor.set_speed(_pwm_lmotor_stop);
                    _rmotor.set_speed(1480); // decrese this
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    // pause for 200 ms
                    _lmotor.set_speed(_pwm_lmotor_stop);
                    _rmotor.set_speed(_pwm_rmotor_stop);
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
            }
            // if aruco target is the the right of the pi cam, move right
            else if (_target_aruco_center.x > 290) {
                while (_target_aruco_center.x > 290) {
                    _lmotor.set_speed(1505); // increase this
                    _rmotor.set_speed(_pwm_rmotor_stop);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    // pause for 200 ms
                    _lmotor.set_speed(_pwm_lmotor_stop);
                    _rmotor.set_speed(_pwm_rmotor_stop);
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
            }

        // pause for 200 ms
//        _lmotor.set_speed(_pwm_lmotor_stop);
//        _rmotor.set_speed(_pwm_rmotor_stop);
//        std::this_thread::sleep_for(std::chrono::milliseconds(400));
//
//        // move forward
//        _lmotor.set_speed(_pwm_lmotor_forward);
//        _rmotor.set_speed(_pwm_rmotor_forward);
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
////        // pause for 400 ms
//        _lmotor.set_speed(_pwm_lmotor_stop);
//        _rmotor.set_speed(_pwm_rmotor_stop);
//        std::this_thread::sleep_for(std::chrono::milliseconds(400));

        //while (std::abs(distance) > 1 && _target_aruco_area < 1500) {
        //while (std::abs(distance) > 1 && _target_aruco_area < 1500) {
        while (distance > forward_stop_offset - 200) {
            // if aruco target is to the left of the pi cam, move left
            if (_target_aruco_center.x < 290) {
                while (_target_aruco_center.x < 290) {
                    _lmotor.set_speed(_pwm_lmotor_stop);
                    _rmotor.set_speed(1480); // decrese this
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    // pause for 200 ms
                    _lmotor.set_speed(_pwm_lmotor_stop);
                    _rmotor.set_speed(_pwm_rmotor_stop);
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));

                    if (distance < forward_stop_offset - 200){
                        break;
                    }

                }
            }
            // if aruco target is the the right of the pi cam, move right
            else if (_target_aruco_center.x > 290) {
                while (_target_aruco_center.x > 290) {
                    _lmotor.set_speed(1505); // increase this
                    _rmotor.set_speed(_pwm_rmotor_stop);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    // pause for 200 ms
                    _lmotor.set_speed(_pwm_lmotor_stop);
                    _rmotor.set_speed(_pwm_rmotor_stop);
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));

                    if (distance < forward_stop_offset - 200){
                        break;
                    }
                }
            }

            _lmotor.set_speed(_pwm_lmotor_stop);
            _rmotor.set_speed(_pwm_rmotor_stop);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            _lmotor.set_speed(_pwm_lmotor_forward);
            _rmotor.set_speed(_pwm_rmotor_forward);
//            if (distance < forward_stop_offset) {
//                _lmotor.set_speed(_pwm_lmotor_stop);
//                _rmotor.set_speed(_pwm_rmotor_stop);
//            }
            std::this_thread::sleep_for(std::chrono::milliseconds(300));

//            if (_target_aruco_area > 420 && _target_aruco_area < 6009)
//                shoot();
//            else
//                shoot_stop();

        }

        _target_aruco_id = 29;
//
//
        // make 90 degree angle at arena corner
        _lmotor.set_speed(_pwm_lmotor_right);
        _rmotor.set_speed(_pwm_rmotor_right);
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
//
        // pause for 200 ms
        _lmotor.set_speed(_pwm_lmotor_stop);
        _rmotor.set_speed(_pwm_rmotor_stop);
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
//

        // course correction at corner before moving straight
        // if aruco target is to the left of the pi cam, move left
        if (_target_aruco_center.x < 290) {
            while (_target_aruco_center.x < 290) {
                _lmotor.set_speed(_pwm_lmotor_stop);
                _rmotor.set_speed(1480); // decrese this
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                // pause for 200 ms
                _lmotor.set_speed(_pwm_lmotor_stop);
                _rmotor.set_speed(_pwm_rmotor_stop);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }
        // if aruco target is the the right of the pi cam, move right
        else if (_target_aruco_center.x > 290) {
            while (_target_aruco_center.x > 290) {
                _lmotor.set_speed(1505); // increase this
                _rmotor.set_speed(_pwm_rmotor_stop);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                // pause for 200 ms
                _lmotor.set_speed(_pwm_lmotor_stop);
                _rmotor.set_speed(_pwm_rmotor_stop);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }
//
//        // move forward
//        _lmotor.set_speed(_pwm_lmotor_forward);
//        _rmotor.set_speed(_pwm_rmotor_forward);
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
////        // pause for 400 ms
//        _lmotor.set_speed(_pwm_lmotor_stop);
//        _rmotor.set_speed(_pwm_rmotor_stop);
//        std::this_thread::sleep_for(std::chrono::milliseconds(400));
//
//        //while (std::abs(distance) > 1 && _target_aruco_area < 1500) {
        while (_target_aruco_area < 615) {
            // if aruco target is to the left of the pi cam, move left
            if (_target_aruco_center.x < 290) {
                while (_target_aruco_center.x < 290) {
                    _lmotor.set_speed(_pwm_lmotor_stop);
                    _rmotor.set_speed(1480); // decrese this
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    // pause for 200 ms
                    _lmotor.set_speed(_pwm_lmotor_stop);
                    _rmotor.set_speed(_pwm_rmotor_stop);
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));

                    if (_target_aruco_area > 615){
                        break;
                    }

                }
            // if aruco target is the the right of the pi cam, move right
            }
            else if (_target_aruco_center.x > 290) {
                while (_target_aruco_center.x > 290) {
                    _lmotor.set_speed(1505); // increase this
                    _rmotor.set_speed(_pwm_rmotor_stop);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    // pause for 200 ms
                    _lmotor.set_speed(_pwm_lmotor_stop);
                    _rmotor.set_speed(_pwm_rmotor_stop);
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));

                    if (_target_aruco_area > 615){
                        break;
                    }

                }
            }

            _lmotor.set_speed(_pwm_lmotor_stop);
            _rmotor.set_speed(_pwm_rmotor_stop);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            _lmotor.set_speed(_pwm_lmotor_forward);
            _rmotor.set_speed(_pwm_rmotor_forward);
//            if (distance < forward_stop_offset) {
//                _lmotor.set_speed(_pwm_lmotor_stop);
//                _rmotor.set_speed(_pwm_rmotor_stop);
//            }
            std::this_thread::sleep_for(std::chrono::milliseconds(300));

//            if (_target_aruco_area > 420 && _target_aruco_area < 6009)
//                shoot();
//            else
//                shoot_stop();

        }

        _target_aruco_id = 32;
//
//
        // make 90 degree angle at arena corner
        _lmotor.set_speed(_pwm_lmotor_right);
        _rmotor.set_speed(_pwm_rmotor_right);
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
//
        // pause for 200 ms
        _lmotor.set_speed(_pwm_lmotor_stop);
        _rmotor.set_speed(_pwm_rmotor_stop);
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
//
//
//        // course correction at corner before moving straight
//        // if aruco target is to the left of the pi cam, move left
//        if (_target_aruco_center.x < 320) {
//            while (_target_aruco_center.x < 320) {
//                _lmotor.set_speed(_pwm_lmotor_stop);
//                _rmotor.set_speed(1480); // decrese this
//                std::this_thread::sleep_for(std::chrono::milliseconds(100));
//                // pause for 200 ms
//                _lmotor.set_speed(_pwm_lmotor_stop);
//                _rmotor.set_speed(_pwm_rmotor_stop);
//                std::this_thread::sleep_for(std::chrono::milliseconds(200));
//            }
//        }
//        // if aruco target is the the right of the pi cam, move right
//        else if (_target_aruco_center.x > 320) {
//            while (_target_aruco_center.x > 320) {
//                _lmotor.set_speed(1505); // increase this
//                _rmotor.set_speed(_pwm_rmotor_stop);
//                std::this_thread::sleep_for(std::chrono::milliseconds(100));
//                // pause for 200 ms
//                _lmotor.set_speed(_pwm_lmotor_stop);
//                _rmotor.set_speed(_pwm_rmotor_stop);
//                std::this_thread::sleep_for(std::chrono::milliseconds(200));
//            }
//        }
//




        _autonomous = 0;
    }
}

void CCar::update_target_aruco() {
    int index = -1;
    for (size_t i = 0; i < ids.size(); ++i) {
        if (ids[i] == _target_aruco_id) {
            index = i;
            break;
        }
    }
    // If ID 4 is found, get its area and center
    if (index != -1 && index < areas.size() && index < centers.size()) {
        _target_aruco_area = areas[index];
        _target_aruco_center = centers[index];
        // Use area and center here as needed
    }
}

void CCar::shoot() {
    gpioServo(GUN_OUTPUT_PIN, _shoot_speed);
}

void CCar::shoot_stop() {
    gpioServo(GUN_OUTPUT_PIN, 1510);
}

void CCar::get_commands() {
    _pi_server.get_cmd(_commands_list);
}


void CCar::draw() {
    _canvas = _persistentCanvas.clone();  // clear canvas
    drawImage(_canvas, cv::Point(3, 3), _processedFrame);

    cvui::window(_canvas, _gui_position.x, _gui_position.y, 600, 600, "CarProject");

    cvui::text(_canvas, 10, 10, "ID TARGET: " + std::to_string(_target_aruco_id), 0.5);


    if (areas.size() != 0) {
        for (size_t i = 0; i < corners.size(); i++) {
            cvui::text(_canvas, 10, 30 + 20 * i, "ID: " + std::to_string(ids.at(i)) + ": " + std::to_string(areas.at(i)) + " - " + std::to_string(centers.at(i).x) + ", " + std::to_string(centers.at(i).y), 0.5);

        }
    }


    cvui::text(_canvas, _gui_position.x + 70, _gui_position.y + _gui_vertspace, "L - Forward");
    cvui::trackbar(_canvas, _gui_position.x + 10, _gui_position.y + _gui_vertspace * 1, 550, &_pwm_lmotor_forward, 500, 2500);
    cvui::text(_canvas, _gui_position.x + 70, _gui_position.y + _gui_vertspace * 2, "R - Forward");
    cvui::trackbar(_canvas, _gui_position.x + 10, _gui_position.y + _gui_vertspace * 2, 550, &_pwm_rmotor_forward, 500, 2500);
    cvui::text(_canvas, _gui_position.x + 70, _gui_position.y + _gui_vertspace * 3, "L - Backward");
    cvui::trackbar(_canvas, _gui_position.x + 10, _gui_position.y + _gui_vertspace * 3, 550, &_pwm_lmotor_backward, 500, 2500);
    cvui::text(_canvas, _gui_position.x + 70, _gui_position.y + _gui_vertspace * 4, "R - Backward");
    cvui::trackbar(_canvas, _gui_position.x + 10, _gui_position.y + _gui_vertspace * 4, 550, &_pwm_rmotor_backward, 500, 2500);
    cvui::text(_canvas, _gui_position.x + 70, _gui_position.y + _gui_vertspace * 5, "L - Left");
    cvui::trackbar(_canvas, _gui_position.x + 10, _gui_position.y + _gui_vertspace * 5, 550, &_pwm_lmotor_left, 500, 2500);
    cvui::text(_canvas, _gui_position.x + 70, _gui_position.y + _gui_vertspace * 6, "R - Left");
    cvui::trackbar(_canvas, _gui_position.x + 10, _gui_position.y + _gui_vertspace * 6, 550, &_pwm_rmotor_left, 500, 2500);
    cvui::text(_canvas, _gui_position.x + 70, _gui_position.y + _gui_vertspace * 7, "L - Right");
    cvui::trackbar(_canvas, _gui_position.x + 10, _gui_position.y + _gui_vertspace * 7, 550, &_pwm_lmotor_right, 500, 2500);
    cvui::text(_canvas, _gui_position.x + 70, _gui_position.y + _gui_vertspace * 8, "R - Right");
    cvui::trackbar(_canvas, _gui_position.x + 10, _gui_position.y + _gui_vertspace * 8, 550, &_pwm_rmotor_right, 500, 2500);
    cvui::text(_canvas, _gui_position.x + 70, _gui_position.y + _gui_vertspace * 9, "L - Stop");
    cvui::trackbar(_canvas, _gui_position.x + 10, _gui_position.y + _gui_vertspace * 9, 550, &_pwm_lmotor_stop, 500, 2500);
    cvui::text(_canvas, _gui_position.x + 70, _gui_position.y + _gui_vertspace * 10, "L - CW");
    cvui::trackbar(_canvas, _gui_position.x + 10, _gui_position.y + _gui_vertspace * 10, 550, &_pwm_lmotor_CW, 500, 2500);
    cvui::text(_canvas, _gui_position.x + 70, _gui_position.y + _gui_vertspace * 10, "R - CW");
    cvui::trackbar(_canvas, _gui_position.x + 10, _gui_position.y + _gui_vertspace * 10, 550, &_pwm_rmotor_CW, 500, 2500);


    // autosort
    cvui::checkbox(_canvas, _gui_position.x + 10, _gui_position.y + 30, "Autonomous", &_autonomous);



        // Display the canvas on the window
    if (!_exitFlag) {
        // Update the CVUI GUI system
        cvui::update();
        cv::imshow("CarProject", _canvas);
    }

}


void CCar::drawImage(cv::Mat& canvas, cv::Point position, cv::Mat& image) {
    cv::Rect imageRegion(position.x, position.y, image.cols, image.rows);
    //cv::rectangle(canvas, cv::Rect(imageRegion.x - 1, imageRegion.y - 1, imageRegion.width + 2, imageRegion.height + 2), cv::Scalar(255, 255, 255), cv::FILLED);
    if(!image.empty()){
        image.copyTo(canvas(imageRegion));
    }
}



void CCar::lmotor_drive() {
    _lmotor.go();
}


void CCar::rmotor_drive() {
    _rmotor.go();
}

void CCar::sense_distance(){

    control_obj.set_digital(TRIGGER_PIN, 1);
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    control_obj.set_digital(TRIGGER_PIN, 0);

    int result = 0;
    control_obj.get_digital(ECHO_PIN, result);

    func_start_time = std::chrono::steady_clock::now();
    while (result == 0) {
        control_obj.get_digital(ECHO_PIN, result);
        pulse_start_time = std::chrono::steady_clock::now();

        func_duration = std::chrono::duration_cast<std::chrono::milliseconds>(pulse_start_time - func_start_time);
        if(func_duration.count() > 100){
            break;
        }
    }
    //std::cout << pulse_end_time;

    func_start_time = std::chrono::steady_clock::now();
    while (result == 1) {
        control_obj.get_digital(ECHO_PIN, result);
        pulse_end_time = std::chrono::steady_clock::now();

        func_duration = std::chrono::duration_cast<std::chrono::milliseconds>(pulse_end_time - func_start_time);
        if(func_duration.count() > 100){
            break;
        }
    }
    //std::cout << " " << pulse_start_time;

    pulse_duration = std::chrono::duration_cast<std::chrono::microseconds>(pulse_end_time - pulse_start_time);
    distance = std::abs(pulse_duration.count());
    if (distance > 10000){
        distance = prev_distance;
    }
    std::cout << "Distance: " << std::abs(distance) << std::endl;
    prev_distance = distance;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void CCar::draw_thread(CCar* ptr) {
    while (ptr->_thread_exit == false) {
        ptr->draw();
    }
}

void CCar::update_thread(CCar* ptr) {
    while (ptr->_thread_exit == false) {
        ptr->update();
    }
}

void CCar::update_target_aruco_thread(CCar* ptr) {
    while (ptr-> _thread_exit == false) {
        ptr-> update_target_aruco();
    }
}


void CCar::autonomous_thread(CCar* ptr) {
    while (ptr->_thread_exit == false) {
        ptr->autonomous();
    }
}

void CCar::lmotor_drive_thread(CCar* ptr) {
    while (ptr->_thread_exit == false) {
        ptr->lmotor_drive();
    }
}

void CCar::rmotor_drive_thread(CCar* ptr) {
    while (ptr->_thread_exit == false) {
        ptr->rmotor_drive();
    }
}

void CCar::ultrasonic_sensor_thread(CCar* ptr) {
    while (ptr->_thread_exit == false) {
        ptr->sense_distance();
    }
}

void CCar::get_commands_thread(CCar* ptr) {
    while(ptr->_thread_exit == false ) {
        ptr->_pi_server.start(4012);
    }
};

void CCar::run() {
    std::thread pi_server_t(&CCar::get_commands_thread, this);
    std::thread lmotor_drive_t(&CCar::lmotor_drive_thread, this);
    std::thread rmotor_drive_t(&CCar::rmotor_drive_thread, this);
    std::thread ultrasonic_sensor_t(&CCar::ultrasonic_sensor_thread, this);

    std::thread autonomous_t(&CCar::autonomous_thread, this);
    std::thread update_target_aruco_t(&CCar::update_target_aruco_thread, this);

    lmotor_drive_t.detach();
    rmotor_drive_t.detach();
    pi_server_t.detach();
    ultrasonic_sensor_t.detach();

    autonomous_t.detach();
    update_target_aruco_t.detach();

    do {
        update();
        draw();
    } while (cv::waitKey(1) != 'q');

    _thread_exit = true;
    _pi_server.stop();

    autonomous_t.join();
    update_target_aruco_t.join();

    lmotor_drive_t.join();
    rmotor_drive_t.join();
    pi_server_t.join();
    ultrasonic_sensor_t.join();
}
