#pragma once


#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <conio.h>

#include "Client.h"
#include "CControl.h"

class CController
{
private:

   std::string server_ip;
   int server_port;

   float timeout_start;
   cv::Mat _canvas;
public:
   CController();
   ~CController();

   void draw();
   void update();
   void handleUserInput();
   void send_command();

   // threaded functions
   void draw_thread();
   void update_thread();
   void handleUserInput_thread();
};

