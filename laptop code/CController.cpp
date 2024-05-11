#include "stdafx.h"
#include "CController.h"

CController::CController(int server) {
	// smc - 192.168.0.104
	// home - 10.0.0.223

	//_pi_server_ip = "192.168.0.117";
	_pi_server_ip = "192.168.137.132";
	_pi_server_port = 4012;
	_arena_server_ip = "192.168.0.101";
	_arena_server_port = 5012; 

	_cmd = -1;

	_timeout_start = cv::getTickCount();

	if (server) {
		_pi_client.connect_socket(_pi_server_ip, _pi_server_port);
	}
	else {
		_arena_client.connect_socket(_arena_server_ip, _arena_server_port);

	}

	_control.init_com(4);

	//_analogX = 0;
	//_analogY = 0;
	//_buttonValue1 = 0;
	//_buttonValue2 = 0;
	//_shoot = 0;

	_is_pi = server;
	_thread_exit = false;
	_autodrive = false;
}


CController::~CController() {

}

void CController::send_command(CClient& client, std::string cmd, std::string ip, int port)
{
	std::string str;

	client.tx_str(cmd);
	//std::cout << "\nClient Tx: " << cmd;
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
					cv::imshow("arena", im);
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
	if (!_autodrive) {
		_control.get_analog(2, _analogX);
		_control.get_analog(26, _analogY);
		_control.get_digital(32, _buttonValue1);
		_control.get_digital(33, _buttonValue2);
	}
}

void CController::sendPiCommands() {
	// S 0 AnalogX AnalogY ButtonValue1 ButtonValue 2
	send_command(_pi_client, "S 0 " + std::to_string(_analogX) + " " + std::to_string(_analogY) + " " + std::to_string(_buttonValue1) + " " + std::to_string(_buttonValue2),
		_pi_server_ip, _pi_server_port);
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


void CController::draw_thread(CController* ptr)
{
	while (ptr->_thread_exit == false) {
		ptr->draw();
	}
}
void CController::update_thread(CController* ptr)
{
	while (ptr->_thread_exit == false) {
		ptr->update();
	}
}

void CController::handleUserInput_thread(CController* ptr)
{
	while (ptr->_thread_exit == false) {
		ptr->handleUserInput();
	}
}

void CController::sendPiCommands_thread(CController* ptr) {
	while (ptr->_thread_exit == false) {
		ptr->sendPiCommands();
	}
}


void CController::getPiImage_thread(CController* ptr)
{
	while (ptr->_thread_exit == false) {
		ptr->getPiImage();
	}
}

void CController::getArenaImage_thread(CController* ptr)
{
	while (ptr->_thread_exit == false) {
		ptr->getArenaImage();
	}
}

void CController::getArenaString_thread(CController* ptr)
{
	while (ptr->_thread_exit == false) {
		ptr->getArenaString();
	}
}

void CController::run()
{
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