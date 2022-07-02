#pragma once
#include <functional>
#include "framework.h"
#include "events.h"

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

	bool operator==(const TcpSocket& other) const;
	bool operator!=(const TcpSocket& other) const;
	bool operator<(const TcpSocket& other) const;
};

class AsyncSocketHandler
{
	HWND hWnd_;
	UINT message_;
public:
	Event<TcpSocket> accepted;
	Event<TcpSocket> received;
	Event<TcpSocket> closed;
	AsyncSocketHandler(HWND hWnd, UINT message);
	void Register(const TcpSocket& socket, long events);
	void OnEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
