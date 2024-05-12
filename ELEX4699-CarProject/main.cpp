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

void print_menu() {
	std::cout << "\n***********************************";
	std::cout << "\n* ELEX4699 Car Project";
	std::cout << "\n***********************************";
    std::cout << "\n(0) Teleoperation";
	std::cout << "\n(1) Autonomous";
	std::cout << "\n(2) Exit";
	std::cout << "\nCMD> ";
}

int main() {
	int cmd = -1;
	do {
	    print_menu();
		std::cin >> cmd;

		if (cmd == 0) {
			CCar mycar(0);
			mycar.run();
		}
		else if (cmd == 1) {
            CCar mycar(1); 
			mycar.run(); 
        }
	} while (cmd != 2);
}
