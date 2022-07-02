#pragma once
#include <map>
#include "tcp_socket.h"
#include "tcp.h"

class TcpClient : public Tcp
{
	TcpSocket socket_;
public:
	TcpClient(const std::string& address, int port);
	virtual void Bind(AsyncSocketHandler& handler) override;
	virtual void Broadcast(const char* buf, int len) override;
	virtual void AcceptedHandler(TcpSocket socket) override;
	virtual void ReceivedHandler(TcpSocket socket) override;
	virtual void ClosedHandler(TcpSocket socket) override;
};