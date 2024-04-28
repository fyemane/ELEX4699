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


#define CANVAS_NAME "Display Image"

std::string server_ip = "192.168.0.100"; /*"10.0.0.223";*/
int server_port = 4012;

float timeout_start;

void print_menu()
{
	std::cout << "\n***********************************";
	std::cout << "\n* ELEX4699 Client Project";
	std::cout << "\n***********************************";
	std::cout << "\n(1) Set system ON";
	std::cout << "\n(2) Set system OFF";
	std::cout << "\n(3) Go backward";
	std::cout << "\n(4) Go forward";
	std::cout << "\n(5) Go left";
	std::cout << "\n(6) Go right";
	std::cout << "\n(7) Stop";
	std::cout << "\n(8) Increase speed";
	std::cout << "\n(9) Decrease speed";
	std::cout << "\n(0) Exit";
	std::cout << "\nCMD> ";
}

void send_command(CClient& client, std::string cmd)
{
	std::string str;

	client.tx_str(cmd);
	std::cout << "\nClient Tx: " << cmd;

	if (cmd == "G 1")
	{
		cv::Mat im;

		if (client.rx_im(im) == true)
		{
			std::cout << "True" << std::endl;
			timeout_start = cv::getTickCount();
			if (im.empty() == false)
			{
				std::cout << "\nClient Rx: Image received";
				cv::imshow("rx", im);
				cv::waitKey(10);
			}
		}
		else
		{
			if ((cv::getTickCount() - timeout_start) / cv::getTickFrequency() > 1000)
			{
				// No response, disconnect and reconnect
				timeout_start = cv::getTickCount();
				client.close_socket();
				client.connect_socket(server_ip, server_port);
			}
		}
	}
	else	{
		if (client.rx_str(str) == true)
		{
			timeout_start = cv::getTickCount();
			std::cout << "\nClient Rx: " << str;
		}
		else
		{
			if ((cv::getTickCount() - timeout_start) / cv::getTickFrequency() > 1000)
			{
				// No response, disconnect and reconnect
				timeout_start = cv::getTickCount();
				client.close_socket();
				client.connect_socket(server_ip, server_port);
			}
		}
	}
}

int main(int argc, char* argv[])
{
	CClient client;
	int cmd = -1;


	timeout_start = cv::getTickCount();
	client.connect_socket(server_ip, server_port);

	CControl control;
	control.init_com(3);

	int analogX = 0, analogY = 0, buttonValue1 = 1, buttonValue2 = 1;

	do
	{
		print_menu();
		std::cin >> cmd;



		switch (cmd)
		{
			//case 1: send_command(client, "S 1"); break; // on
			//case 2: send_command(client, "S 0"); break; // off
		case 2: send_command(client, "S 0 0"); break; // backward
		case 8: send_command(client, "S 0 1"); break; // forward
		case 4: send_command(client, "S 0 2"); break; // left
		case 6: send_command(client, "S 0 3"); break; // right
		case 5: send_command(client, "S 0 4"); break; // stop
			//case 8: send_command(client, "S 0 5"); break; // speed up
		case 9: send_command(client, "S 0 6"); break; // slow down
		case 7: while (true) {
			control.get_analog(2, analogX);
			control.get_analog(26, analogY);
			control.get_digital(32, buttonValue1);
			control.get_digital(33, buttonValue2);
			send_command(client, "S 0 " + std::to_string(analogX) + " " + std::to_string(analogY) + " " + std::to_string(!buttonValue1) + " " + std::to_string(!buttonValue2));
		}
		case 1: while (true) { send_command(client, "G 1"); cv::waitKey(20); }; break;
		}
	} while (cmd != 0);
} 