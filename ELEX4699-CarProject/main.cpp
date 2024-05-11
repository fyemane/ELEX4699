////////////////////////////////////////////////////////////////
// ELEX 4699  project for BCIT
// Created April 23, 2024 by Raghav Marwah and Faniel Yemane
// Last updated April 23, 2024
////////////////////////////////////////////////////////////////
// Add simple GUI elements
#define CVUI_DISABLE_COMPILATION_NOTICES
#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <unistd.h>

#include "CControlPi.h"
#include "CCar.h"

#define CANVAS_NAME "Display Image"

void lab10() {
    CCar mycar;
    mycar.run();
}

void print_menu() {
	std::cout << "\n***********************************";
	std::cout << "\n* ELEX4618 Template Project";
	std::cout << "\n***********************************";
    std::cout << "\n(1) Lab 10 - User Input";
	std::cout << "\n(0) Exit";
	std::cout << "\nCMD> ";
}

int main() {
	int cmd = -1;
	do {
	    print_menu();
		std::cin >> cmd;
		switch (cmd) {
            case 1: lab10(); break;
        }
	} while (cmd != 0);
}
