#pragma once
#include "framework.h"

class WsaHolder
{
	static WSAData data_;
	static int counter;
public:
	WSAData data() const;
	WsaHolder();
	~WsaHolder();
};

class TcpSocket
{
	SOCKET socket_ = INVALID_SOCKET;
	static WsaHolder wsa;
public:
	TcpSocket();
	TcpSocket(SOCKET socket);
	~TcpSocket();
	bool Bind(const std::string& address, int port);
	bool Listen(int max_queue = SOMAXCONN);
	TcpSocket Accept();
	bool Connect(const std::string& address, int port);
	void Send(const char* buf, int len, int flags = 0);
	int Receive(char* buf, int len, int flags = 0);
	bool IsValid() const;
	SOCKET RawSocket() const;
};
