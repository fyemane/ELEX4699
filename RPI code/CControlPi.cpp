#include <iostream>
#include <unistd.h>

#include "pigpio.h"
#include "CControlPi.h"


CControlPi::CControlPi() {
    if (gpioInitialise() < 0) {
        std::cout << "Failed to initialize pigpio library" << std::endl;
    } else {
        std::cout << "Initialization is good" << std::endl;
    }
}
//
CControlPi::~CControlPi() {
    gpioTerminate();
}
//
//void CControl::init_com(int comport) {
//
//}
//

bool CControlPi::get_analog(int channel, int& result) {
    // Function to read analog input from MCP3008 using pigpio
    // Check if channel is within valid range
    if (channel < 0 || channel > 7) {
        std::cout << "Channel is outside valid range" << std::endl;
        return false; // invalid channel
    }

    // Open SPI device
    int handle = spiOpen(0, 200000, 3); // Mode 0, 200 Khz]
    if (handle < 0) {
        std::cout << "Failed to open SPI" << std::endl;
        return false; // failed to open SPI
    }

    // SPI transaction command to read from MCP3008
    char cmd[] = {1, (8 + channel) << 4, 0}; // 0b1xxx0000 where xxx = channel 0
    unsigned char inBuf[3]; // buffer to store received data

    // Perform SPI transfer
    int result_code = spiXfer(handle, cmd, (char*) inBuf, 3); // Transfer 3 bytes
    if (result_code < 0) {
        std::cout << "SPI transfer failed" << std::endl;
        spiClose(handle);
        return false; // SPI transfer failed
    }

    // Extract analog value from received data
    result = ((inBuf[1] & 3)) << 8 | inBuf[2]; // format 10 bits
    spiClose(handle); // Close SPI system

    return true; // Analog read successful
}


void CControlPi::get_digital(int channel, int& result) {
    gpioSetMode(channel, PI_INPUT);
    result = gpioRead(channel);
}
//
void CControlPi::set_digital(int channel, int val) {
    gpioSetMode(channel, PI_OUTPUT);
    gpioWrite(channel, val);
}
//
void CControlPi::get_servo(int channel, int& result) {

}
//
void CControlPi::set_servo(int channel, int position) {
    gpioSetMode(channel, PI_OUTPUT);
    gpioServo(channel, position);
    usleep(1000);
}

//
bool CControlPi::get_button(int channel, int& val) {
    gpioSetMode(channel, PI_INPUT);
    val = gpioRead(channel);
}




