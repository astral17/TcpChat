#pragma once
#include "framework.h"
#include "tcp_socket.h"
// Server, info about all clients, send/receive messages
class TcpServer
{
	TcpSocket socket;
public:
	TcpServer(const std::string& address, int port);
	void* GetClient(int id);
	bool Send(void* client, const char* buf, int len);
	int Receive(char* buf, int len);
};