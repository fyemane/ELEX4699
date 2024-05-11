#include "CCar.h"
#include "cvui.h"

CCar::CCar() {
    _pi_server_port = 4012;

    _thread_exit = false;
    _resetFlag = false;
    _exitFlag = false;

    _shoot_speed = 500;

    _deadzone = .1;

    // for drawing
    _canvasSize = cv::Size(700, 700);
    _canvas = cv::Mat::zeros(_canvasSize, CV_8UC3);
    _persistentCanvas = cv::Mat::zeros(_canvasSize, CV_8UC3);  // Initialize persistent canvas
    cvui::init("CarProject");              // Initialize cvui here

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
    _lmotor.set_speed(1480);
    _rmotor.set_speed(1500);
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

        // detect markers and update id list and vector list
        // cv::aruco::detectMarkers(_vidFrame, dictionary, corners, ids);
        // if (ids.size() > 0) {
        //     cv::aruco::drawDetectedMarkers(_processedFrame, corners, ids);
        // }
    }

    _pi_server.get_cmd(_commands_list);

    if (!_commands_list.empty()) {
        for (const auto& str: _commands_list) {
            std::cout << "Received: " << str << std::endl;
        }
        std::istringstream iss(_commands_list.at(0));

        // parse string
        iss >> cmd1 >> cmd2 >> cmd3 >> cmd4 >> cmd5 >> cmd6;

        _analogX = cmd3 / 4096.0;
        _analogY  = cmd4 / 4096.0;

        if(cmd1 != "G"){
            if (_analogY > 0.4 && _analogY < 0.6 && _analogX > 0.4 && _analogX < 0.6) {
            // stop
                _lmotor.set_speed(1480);
                _rmotor.set_speed(1500);
            }
            else if (_analogY > _analogX && _analogY > 1 - _analogX) {
            // forward
                _lmotor.set_speed(1545);
                _rmotor.set_speed(500);
            }
            else if (_analogY < _analogX && _analogY < 1 - _analogX) {
                // backward
                _lmotor.set_speed(1405);
                _rmotor.set_speed(2500);
            }
            else if (_analogY > _analogX && _analogY < 1 - _analogX) {
                // left
                _lmotor.set_speed(500);
                _rmotor.set_speed(500);
            }
            // right
            else if (_analogY < _analogX && _analogY > 1 - _analogX) {
                _lmotor.set_speed(2500);
                _rmotor.set_speed(2500);
            }
            // stop
            // else {
            //     _lmotor.set_speed(1500);
            //     _rmotor.set_speed(1500);
            // }

            if (cmd5 == 0) {
                shoot();
            }
            else {
                shoot_stop();
            }
        }  
    }
}

void CCar::shoot() {
    gpioServo(GUN_OUTPUT_PIN, _shoot_speed);
}

void CCar::shoot_stop() {
    gpioServo(GUN_OUTPUT_PIN, 1500);
}

void CCar::get_commands() {
    _pi_server.get_cmd(_commands_list);
}


void CCar::draw() {
    _canvas = _persistentCanvas.clone();  // clear canvas
    drawImage(_canvas, cv::Point(3, 3), _processedFrame);

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

void CCar::draw_thread(CCar* ptr) {
    while (ptr->_thread_exit == false) {
        ptr->draw();
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

void CCar::get_commands_thread(CCar* ptr) {
    while(ptr->_thread_exit == false ) {
        ptr->_pi_server.start(4012);
    }
};

void CCar::run() {
    std::thread pi_server_t(&CCar::get_commands_thread, this);
    std::thread lmotor_drive_t(&CCar::lmotor_drive_thread, this);
    std::thread rmotor_drive_t(&CCar::rmotor_drive_thread, this);

    lmotor_drive_t.detach();
    rmotor_drive_t.detach();
    pi_server_t.detach();

    do {
        update();
        draw();
    } while (cv::waitKey(1) != 'q');

    _thread_exit = true;
    _pi_server.stop();

    lmotor_drive_t.join();
    rmotor_drive_t.join();
    pi_server_t.join();
}
