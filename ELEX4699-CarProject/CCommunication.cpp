#include <algorithm>
#include <chrono>
#include <thread>

#include "Constants.h"
#include "CCommunication.h"


CCommunication::CCommunication() {
    arena_server_ip = "192.168.0.101";
    pi_server_ip = "10.0.0.223";

    pi_server_port = 4012;
    arena_server_port = 5012;



}

CCommunication::~CCommunication() {

}


void CCommunication::start_pi_server(int port) {
    pi_server.start(port);

}

void CCommunication::get_command(std::vector<std::string> commands_list) {
    pi_server.get_cmd(commands_list);
}


void CCommunication::send_command(CClient& client, std::string cmd)
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
				client.connect_socket(pi_server_ip, pi_server_port);
			}
		}
	}
	else
	{
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
				client.connect_socket(pi_server_ip, pi_server_port);
			}
		}
	}
}
