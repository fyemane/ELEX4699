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
#include "CController.h"

#define CANVAS_NAME "Display Image"

void print_menu()
{
	std::cout << "\n***********************************";
	std::cout << "\n* ELEX4699 Client Project";
	std::cout << "\n***********************************";
	std::cout << "\n(1) Pi client";
	std::cout << "\n(0) Arena client";
	std::cout << "\nCMD> ";
}

int main(int argc, char* argv[])
{
   int userInput;
   print_menu();
   std::cin >> userInput;
   //userInput = 1;

   if (userInput == 0 || userInput == 1)
   {
      CController controller(userInput);
      controller.run();
   }
   else
   {
      std::cout << "Invalid input! Please enter either 0 or 1." << std::endl;
   }

   return 0;
} 