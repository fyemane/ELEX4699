#pragma once

#include "Client.h"
#include "server.h"

#include <thread>
#include <iostream>
#include <chrono>


class CCommunication
{
private:
    CServer pi_server;
    CClient pi_client;

    std::string arena_server_ip;
    std::string pi_server_ip;

    float timeout_start;

    int pi_server_port;
    int arena_server_port;


public:
    CCommunication();
    ~CCommunication();

    // client commands
    void send_command(CClient& client, std::string cmd);



    // server comands
    void get_command(std::vector<std::string> commands_list);
    void start_pi_server(int port);

};
