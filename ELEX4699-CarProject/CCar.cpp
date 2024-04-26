#include "server.h"
#include "CCar.h"

CCar::CCar() {
    _arena_server_ip = "192.168.0.101";
    _pi_server_ip = "10.0.0.223";

    _pi_server_port = 4012;
    _arena_server_port = 5012;

    _thread_exit = false;

    _analogX = 0;
    _analogY = 0;


    if (gpioInitialise() < 0) {
        std::cerr << "GPIO initialization failed" << std::endl;
    }
}

CCar::~CCar() {
    gpioTerminate();

}

void CCar::update() {
    _pi_server.get_cmd(_commands_list);
    if (!_commands_list.empty()) {
        for (const auto& str: _commands_list) {
            std::cout << "Received: " << str << std::endl;
        }
        std::istringstream iss(_commands_list.at(0));

        // parse string
        iss >> cmd1 >> cmd2 >> cmd3 >> cmd4 >> cmd5 >> cmd6;

//        _analogX = cmd3 / 4096.0 * 2000 + 500;
//        _analogY = cmd4 / 4096.0 * 2000 + 500;
        _analogY  = cmd4 / 4096.0;

        if (_analogY > 0.6) {
            // forward
            _lmotor.set_speed(2500);
            _rmotor.set_speed(1417);
        }
        else if (_analogY < 0.4) {
            // backward
            _lmotor.set_speed(500);
            _rmotor.set_speed(1565);
        }
        // stop
        else {
            _lmotor.set_speed(1500);
            _rmotor.set_speed(1490);
        }
        // left
        if (cmd5 == 1) {
            _lmotor.set_speed(2500);
            _rmotor.set_speed(1500);
        }
        // right
        if (cmd6 == 1) {
            _lmotor.set_speed(1500);
            _rmotor.set_speed(500);
        }



//
//        std::cout << "Analog X: " << std::to_string(_analogX) << " Analog Y: " << std::to_string(_analogY) << std::endl;
//

//        _lmotor.set_speed(_analogX);
//        _rmotor.set_speed(_analogY);

        // backward
        if (_commands_list.at(0) == "S 0 0") {
            _lmotor.set_speed(500);
            _rmotor.set_speed(1565);
        }
        // forward
        if (_commands_list.at(0) == "S 0 1") {
            _lmotor.set_speed(2500);
            _rmotor.set_speed(1417);
        }
        // left
        if (_commands_list.at(0) == "S 0 2") {
            _lmotor.set_speed(1500);
            _rmotor.set_speed(500);
        }
        // right
        if (_commands_list.at(0) == "S 0 3") {
            _lmotor.set_speed(2500);
            _rmotor.set_speed(1500);
        }
        // stop
        if (_commands_list.at(0) == "S 0 4") {
            _lmotor.set_speed(1500);
            _rmotor.set_speed(1490);
        }
        // speed up
        if (_commands_list.at(0) == "S 0 5") {
            if (_lmotor.get_speed() > 1500 && _lmotor.get_speed() < 2500)
                _lmotor.set_speed(_lmotor.get_speed() + 100);
            if (_rmotor.get_speed() > 1500 && _rmotor.get_speed() < 2500)
                _rmotor.set_speed(_rmotor.get_speed() + 100);
            if (_lmotor.get_speed() < 1500 && _lmotor.get_speed() > 500)
                _lmotor.set_speed(_lmotor.get_speed() - 100);
            if (_rmotor.get_speed() < 1500 && _rmotor.get_speed() > 500)
                _rmotor.set_speed(_rmotor.get_speed() - 100);
        }
        // slow down
        if (_commands_list.at(0) == "S 0 6") {
            if (_lmotor.get_speed() > 1500 && _lmotor.get_speed() < 2500)
                _lmotor.set_speed(_lmotor.get_speed() - 100);
            if (_rmotor.get_speed() > 1500 && _rmotor.get_speed() < 2500)
                _rmotor.set_speed(_rmotor.get_speed() - 100);
            if (_lmotor.get_speed() < 1500 && _lmotor.get_speed() > 500)
                _lmotor.set_speed(_lmotor.get_speed() + 100);
            if (_rmotor.get_speed() < 1500 && _rmotor.get_speed() > 500)
                _rmotor.set_speed(_rmotor.get_speed() + 100);
        }

    }
}


void CCar::lmotor_drive() {
    _lmotor.go();
}


void CCar::rmotor_drive() {
    _rmotor.go();
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

void CCar::serverstart_thread(CCar* ptr) {
    while (ptr->_thread_exit == false) {
        ptr->_pi_server.start(4012);
    }
}



void CCar::run() {
    std::thread pi_server_t(&CCar::serverstart_thread, this);
    std::thread lmotor_drive_t(&CCar::lmotor_drive_thread, this);
    std::thread rmotor_drive_t(&CCar::rmotor_drive_thread, this);

    lmotor_drive_t.detach();
    rmotor_drive_t.detach();
    pi_server_t.detach();

    do {
        update();
    } while (cv::waitKey(1) != 'q');

    _thread_exit = true;
    _pi_server.stop();

    lmotor_drive_t.join();
    rmotor_drive_t.join();
    pi_server_t.join();
}
