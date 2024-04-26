#include "server.h"
#include "CCar.h"

CCar::CCar() {
    _thread_exit = false;

    if (gpioInitialise() < 0) {
        std::cerr << "GPIO initialization failed" << std::endl;
    }
}

CCar::~CCar() {
    gpioTerminate();

}

void CCar::update() {
    _lmotor.set_speed(.3);
    _lmotor.set_duration(3000);
    _rmotor.set_speed(.3);
    _rmotor.set_duration(3000);
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(3000)));

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

void CCar::run() {

    std::thread lmotor_drive_t(&CCar::lmotor_drive_thread, this);
    std::thread rmotor_drive_t(&CCar::rmotor_drive_thread, this);

    lmotor_drive_t.detach();
    rmotor_drive_t.detach();

    do {
        update();
    } while (cv::waitKey(1) != 'q');

    _thread_exit = true;

    lmotor_drive_t.join();
    rmotor_drive_t.join();
}
