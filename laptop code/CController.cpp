#include "stdafx.h"
#include "CController.h"

CController::CController() {
	_server_ip = "10.0.0.223"; /*"10.0.0.223";*/
	_server_port = 4012;

	_cmd = -1;

	_timeout_start = cv::getTickCount();
	_client.connect_socket(_server_ip, _server_port);

	_control.init_com(3);

	_analogX = 0;
	_analogY = 0;
	_buttonValue1 = 0;
	_buttonValue2 = 0;

	_thread_exit = false;
}


CController::~CController() {

}

void CController::send_command(CClient& client, std::string cmd)
{
	std::string str;

	client.tx_str(cmd);
	std::cout << "\nClient Tx: " << cmd;

	//if (cmd == "G 1")
	//{
	//	cv::Mat im;

	//	if (client.rx_im(im) == true)
	//	{
	//		std::cout << "True" << std::endl;
	//		_timeout_start = cv::getTickCount();
	//		if (im.empty() == false)
	//		{
	//			std::cout << "\nClient Rx: Image received";
	//			cv::imshow("rx", im);
	//			cv::waitKey(10);
	//		}
	//	}
	//	else
	//	{
	//		if ((cv::getTickCount() - _timeout_start) / cv::getTickFrequency() > 1000)
	//		{
	//			// No response, disconnect and reconnect
	//			_timeout_start = cv::getTickCount();
	//			client.close_socket();
	//			client.connect_socket(_server_ip, _server_port);
	//		}
	//	}
	//}
	//else {
	//	if (client.rx_str(str) == true)
	//	{
	//		_timeout_start = cv::getTickCount();
	//		std::cout << "\nClient Rx: " << str;
	//	}
	//	else
	//	{
	//		if ((cv::getTickCount() - _timeout_start) / cv::getTickFrequency() > 1000)
	//		{
	//			// No response, disconnect and reconnect
	//			_timeout_start = cv::getTickCount();
	//			client.close_socket();
	//			client.connect_socket(_server_ip, _server_port);
	//		}
	//	}
	//}
}

void CController::update() {

}

void CController::draw() {

}

void CController::handleUserInput() {
	_control.get_analog(2, _analogX);
	_control.get_analog(26, _analogY);
	_control.get_digital(32, _buttonValue1);
	_control.get_digital(33, _buttonValue2);
}

void CController::sendUserCommands() {
	send_command(_client, "S 0 " + std::to_string(_analogX) + " " + std::to_string(_analogY) + " " + std::to_string(!_buttonValue1) + " " + std::to_string(!_buttonValue2));
}

void CController::sendUserCommands_thread(CController* ptr) {
	while (ptr->_thread_exit == false) {
		ptr->sendUserCommands();
	}
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

void CController::run()
{
	// create separate joins
	std::thread update_t(&CController::update_thread, this);
	std::thread draw_t(&CController::draw_thread, this);
	std::thread handleUserInput_t(&CController::handleUserInput_thread, this);
	std::thread sendUserCommands_t(&CController::sendUserCommands_thread, this);

	
	// threads detach
	update_t.detach();
	draw_t.detach();
	handleUserInput_t.detach();
	sendUserCommands_t.detach();

	while (cv::waitKey(1) != 'q') {
		cv::waitKey(1);
	}

	// once program ends, join threads
	_thread_exit = true;
	update_t.join();
	draw_t.join();
	handleUserInput_t.join();
	sendUserCommands_t.join();
}