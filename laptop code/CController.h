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
   CClient _client;

   std::string _server_ip;

   float _timeout_start;

   CControl _control;

   int _server_port;

   int _cmd;
   int _analogX;
   int _analogY;
   int _buttonValue1;
   int _buttonValue2;

   cv::Mat _canvas;

   bool _thread_exit;

public:
   CController();
   ~CController();

   void send_command(CClient& client, std::string cmd);
   void run();

   // functions to be threaded
   void draw();
   void update();
   void handleUserInput();
   void sendUserCommands();

   // threaded functions
   static void draw_thread(CController* ptr);
   static void update_thread(CController* ptr);
   static void handleUserInput_thread(CController* ptr);
   static void sendUserCommands_thread(CController* ptr);
};

