#pragma once
#include "tcp_socket.h"

class Chat
{
	EventHandler<TcpSocket> accepted_handler_;
	EventHandler<TcpSocket> received_handler_;
	EventHandler<TcpSocket> closed_handler_;
public:
	virtual void Bind(AsyncSocketHandler& handler);
	virtual void Broadcast(const char* buf, int len) = 0;
	virtual void AcceptedHandler(TcpSocket socket) = 0;
	virtual void ReceivedHandler(TcpSocket socket) = 0;
	virtual void ClosedHandler(TcpSocket socket) = 0;
};
