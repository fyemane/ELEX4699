////////////////////////////////////////////////////////////////
// ELEX 4618 Client Template project for BCIT
// Created Oct 5, 2016 by Craig Hennessey
// Last updated April 2022
////////////////////////////////////////////////////////////////
#include "stdafx.h"

// Add simple GUI elements
#define CVUI_DISABLE_COMPILATION_NOTICES
#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <conio.h>

#include "Client.h"
#include "CControl.h"
#include "CController.h"

#define CANVAS_NAME "Display Image"


void print_menu()
{
	std::cout << "\n***********************************";
	std::cout << "\n* ELEX4699 Client Project";
	std::cout << "\n***********************************";
	std::cout << "\n(1) Turn on controller input";
	std::cout << "\n(0) Exit";
	std::cout << "\nCMD> ";
}

int main(int argc, char* argv[])
{
	CController controller;
	controller.run();
} 