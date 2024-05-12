#include "stdafx.h"
#include "CController.h"

CController::CController(int server) {
	// smc - 192.168.0.104
	// home - 10.0.0.223

	_pi_server_ip = "192.168.0.117";
	//_pi_server_ip = "192.168.137.132";
	_pi_server_port = 4012;
	_arena_server_ip = "192.168.0.100";
	_arena_server_port = 5012; 

	_cmd = -1;
	_arena_text_size = 0.8;

	_timeout_start = cv::getTickCount();

	if (server) {
		_pi_client.connect_socket(_pi_server_ip, _pi_server_port);
	}
	else {
		_arena_client.connect_socket(_arena_server_ip, _arena_server_port);
	}

	ZeroMemory(&state, sizeof(XINPUT_STATE));
	dwResult = 0;

	_analogLX = 0;
	_analogLY = 0;
	_analogRX = 0;
	_analogRY = 0;
	_buttonX = 0;
	_buttonY = 0;
	_buttonB = 0;
	_buttonA = 0;
	_shoot = 0;

	_is_pi = server;
	_thread_exit = false;
	_autodrive = false;
}


CController::~CController() {

}

void CController::send_command(CClient& client, std::string cmd, std::string ip, int port)
{
	client.tx_str(cmd);
	std::cout << "\nClient Tx: " << cmd;
	cv::waitKey(50);

	if (cmd == "G 1")
	{
		if (client.rx_im(im) == true)
		{
			//std::cout << "True" << std::endl;
			_timeout_start = cv::getTickCount();
			if (im.empty() == false)
			{
				//std::cout << "\nClient Rx: Image received";
				//std::lock_guard<std::mutex> lock(_display_mutex); // Lock access to imshow
				if (port == _pi_server_port) {
					cv::imshow("pi", im);
				}
				else {
					//cv::waitKey(50);
					// Check if str has the correct beginning
					if (str.find("<ARENA TIME=\"") == 0) {
						// Parse the received string
						std::size_t pos_start = str.find("TIME=\"") + 6;
						std::size_t pos_end = str.find("\"", pos_start);
						std::string time_str = str.substr(pos_start, pos_end - pos_start);

						pos_start = str.find("D1=\"") + 4;
						pos_end = str.find("\"", pos_start);
						std::string d1_str = str.substr(pos_start, pos_end - pos_start);

						pos_start = str.find("D2=\"") + 4;
						pos_end = str.find("\"", pos_start);
						std::string d2_str = str.substr(pos_start, pos_end - pos_start);

						pos_start = str.find("D3=\"") + 4;
						pos_end = str.find("\"", pos_start);
						std::string d3_str = str.substr(pos_start, pos_end - pos_start);

						pos_start = str.find("D4=\"") + 4;
						pos_end = str.find("\"", pos_start);
						std::string d4_str = str.substr(pos_start, pos_end - pos_start);

						std::cout << "TIME: " << time << "s D1: " << d1_str << " D2: " << d2_str << " D3: " << d3_str << " D4: " << d4_str << std::endl;

						// Overlay text on the image
						cv::putText(im, time_str, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6, (time_str == "0.000000") ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), 2);
						cv::putText(im, "D1: " + d1_str, cv::Point(0, 260), cv::FONT_HERSHEY_SIMPLEX, 0.6, (d1_str == "0") ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), 2);
						cv::putText(im, "D2: " + d2_str, cv::Point(220, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6, (d2_str == "0") ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), 2);
						cv::putText(im, "D3: " + d3_str, cv::Point(530, 260), cv::FONT_HERSHEY_SIMPLEX, 0.6, (d3_str == "0") ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), 2);
						cv::putText(im, "D4: " + d4_str, cv::Point(220, 590), cv::FONT_HERSHEY_SIMPLEX, 0.6, (d4_str == "0") ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), 2);

						cv::imshow("arena", im);
					}
					}
				}
			}
		else
		{
			if ((cv::getTickCount() - _timeout_start) / cv::getTickFrequency() > 1000)
			{
				// No response, disconnect and reconnect
				_timeout_start = cv::getTickCount();
				client.close_socket();
				client.connect_socket(ip, port);
			}
		}
	}
	else {
		if (port != _pi_server_port) {
			if (client.rx_str(str) == true)
			{
				_timeout_start = cv::getTickCount();
				std::cout << "\nClient Rx: " << str;
			}
			else
			{
				if ((cv::getTickCount() - _timeout_start) / cv::getTickFrequency() > 1000)
				{
					// No response, disconnect and reconnect
					_timeout_start = cv::getTickCount();
					client.close_socket();
					client.connect_socket(ip, port);
				}
			}
		}
	}
}

void CController::send_command_locked(CClient& client, std::string cmd, std::string ip, int port)
{
	std::lock_guard<std::mutex> lock(_send_mutex); // Lock access to send command
	send_command(client, cmd, ip, port);
}

void CController::update() {

}

void CController::draw() {

}

void CController::handleUserInput() {
	
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	dwResult = XInputGetState(0, &state);

	if (dwResult == ERROR_SUCCESS) {
		// Controller is connected
		//std::cout << "Controller detected!\n";

		//_analogLX = state.Gamepad.sThumbLX;
		_analogLY = state.Gamepad.sThumbLY;
		_analogRX = state.Gamepad.sThumbRX;
		//_analogRY = state.Gamepad.sThumbRY;

		_buttonX = state.Gamepad.wButtons & _XINPUT_GAMEPAD_X;
		_buttonY = state.Gamepad.wButtons & _XINPUT_GAMEPAD_Y;
		//_buttonB = state.Gamepad.wButtons & _XINPUT_GAMEPAD_B;

		//std::cout << "LY: " << _analogLY << "   RX: " << _analogRX << "   X: " << _buttonX << "   Y: " << _buttonY << "   B: " << _buttonB << std::endl;
	}
	else {
		// Controller is not connected
		std::cout << "Xbox controller not detected...";
	}

}

void CController::sendPiCommands() {
	// S 0 AnalogLY AnalogRX ButtonX ButtonY
	send_command(_pi_client, "S 0 " + std::to_string(_analogLY) + " " + std::to_string(_analogRX) + " " + std::to_string(_buttonX) + " " + std::to_string(_buttonY), _pi_server_ip, _pi_server_port);
	send_command(_pi_client, "G 1", _pi_server_ip, _pi_server_port);
}

void CController::getPiImage() {
	send_command(_pi_client, "G 1", _pi_server_ip, _pi_server_port);
}

void CController::getArenaImage() {
}

void CController::getArenaString() {
	send_command(_arena_client, "G 1", _arena_server_ip, _arena_server_port);
	send_command(_arena_client, "G 0", _arena_server_ip, _arena_server_port);
}


void CController::draw_thread(CController* ptr) {
	while (ptr->_thread_exit == false) {
		ptr->draw();
	}
}
void CController::update_thread(CController* ptr) {
	while (ptr->_thread_exit == false) {
		ptr->update();
	}
}

void CController::handleUserInput_thread(CController* ptr) {
	while (ptr->_thread_exit == false) {
		ptr->handleUserInput();
	}
}

void CController::sendPiCommands_thread(CController* ptr) {
	while (ptr->_thread_exit == false) {
		ptr->sendPiCommands();
	}
}


void CController::getPiImage_thread(CController* ptr) {
	while (ptr->_thread_exit == false) {
		ptr->getPiImage();
	}
}

void CController::getArenaImage_thread(CController* ptr) {
	while (ptr->_thread_exit == false) {
		ptr->getArenaImage();
	}
}

void CController::getArenaString_thread(CController* ptr) {
	while (ptr->_thread_exit == false) {
		ptr->getArenaString();
	}
}

void CController::run() {
	if (_is_pi) {
		// create separate joins
		std::thread handleUserInput_t(&CController::handleUserInput_thread, this);
		std::thread sendPiCommands_t(&CController::sendPiCommands_thread, this);
		//std::thread getPiImage_t(&CController::getPiImage_thread, this);

		// threads detach
		handleUserInput_t.detach();
		sendPiCommands_t.detach();
		//getPiImage_t.detach();

		do {
			//sendPiCommands();
			update();
			draw();

		} while (cv::waitKey(1) != 'q');

		// once program ends, join threads
		_thread_exit = true;
		handleUserInput_t.join();
		sendPiCommands_t.join();
		//getPiImage_t.join();

	}
	else {
		// create separate joins
		std::thread handleUserInput_t(&CController::handleUserInput_thread, this);
		//std::thread getArenaImage_t(&CController::getArenaImage_thread, this);
		std::thread getArenaString_t(&CController::getArenaString_thread, this);

		// threads detach
		handleUserInput_t.detach();

		//getArenaImage_t.detach();
		getArenaString_t.detach();

		do {
			//sendPiCommands();
			update();
			draw();

		} while (cv::waitKey(1) != 'q');

		// once program ends, join threads
		_thread_exit = true;
		handleUserInput_t.join();
		//getArenaImage_t.join();
		getArenaString_t.join();
	}

}