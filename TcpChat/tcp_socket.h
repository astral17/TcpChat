#pragma once
#include "framework.h"
#include <functional>

class WsaHolder
{
	static WSAData data_;
	static int counter_;
public:
	WSAData data() const;
	WsaHolder();
	~WsaHolder();
};

class TcpSocket
{
	SOCKET socket_;
	static WsaHolder wsa_;
public:
	TcpSocket(bool init = false);
	TcpSocket(const TcpSocket& other);
	TcpSocket(SOCKET socket);
	bool Bind(const std::string& address, int port);
	bool Listen(int max_queue = SOMAXCONN);
	TcpSocket Accept();
	bool Connect(const std::string& address, int port);
	bool Close();
	void Send(const char* buf, int len, int flags = 0);
	int Receive(char* buf, int len, int flags = 0);
	bool IsValid() const;
	SOCKET RawSocket() const;
};

class AsyncSocketHandler
{
	HWND hWnd_;
	UINT message_;
public:
	std::function<void(TcpSocket)> on_accept;
	std::function<void(TcpSocket)> on_message;
	std::function<void(TcpSocket)> on_close;
	AsyncSocketHandler(HWND hWnd, UINT message);
	void Register(const TcpSocket& socket, long events);
	void OnEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
