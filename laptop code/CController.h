#pragma once

#define _AMD64_ 1

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <conio.h>
//#include <windows.h>
#include <Xinput.h>

#include "Client.h"
#include "CControl.h"

class CController {
private:
   CClient _pi_client, _arena_client;

   std::string _pi_server_ip, _arena_server_ip;

   float _timeout_start;

   CControl _control;

   int _pi_server_port, _arena_server_port;

   int _cmd;
   int _analogX;
   int _analogY;
   int _buttonValue1;
   int _buttonValue2;
   int _shoot;

   DWORD dwResult;

   cv::Mat im;

   bool _thread_exit;
   bool _autodrive;
   bool _is_pi;

   std::mutex _display_mutex;
   std::mutex _send_mutex;


public:
   CController(int server);
   ~CController();

   void send_command(CClient& client, std::string cmd, std::string ip, int port);
   void send_command_locked(CClient& client, std::string cmd, std::string ip, int port);
   void run();

   // functions to be threaded
   void draw();
   void update();
   void handleUserInput();

   void getPiImage();
   void sendPiCommands();
   void getArenaImage();
   void getArenaString();

   // threaded functions
   static void draw_thread(CController* ptr);
   static void update_thread(CController* ptr);
   static void handleUserInput_thread(CController* ptr);

   static void getPiImage_thread(CController* ptr);
   static void sendPiCommands_thread(CController* ptr);
   static void getArenaImage_thread(CController* ptr);
   static void getArenaString_thread(CController* ptr);
};

