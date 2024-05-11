#pragma once

#define _AMD64_ 1
#define _XINPUT_GAMEPAD_A	0x1000
#define _XINPUT_GAMEPAD_B	0x2000
#define _XINPUT_GAMEPAD_X	0x4000
#define _XINPUT_GAMEPAD_Y	0x8000

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <conio.h>
#include <Xinput.h>

#include "Client.h"

class CController {
private:
   CClient _pi_client, _arena_client;

   std::string _pi_server_ip, _arena_server_ip;

   float _timeout_start;

   int _pi_server_port, _arena_server_port;

   int _cmd;
   int _analogLX;
   int _analogLY;
   int _analogRX;
   int _analogRY;
   int _buttonX;
   int _buttonY;
   int _buttonB;
   int _buttonA;
   int _shoot;
   int _arena_text_size;

   DWORD dwResult;
   XINPUT_STATE state;

   std::string str;
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

