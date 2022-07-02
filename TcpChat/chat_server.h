#pragma once
#include <map>
#include "tcp_socket.h"
#include "chat.h"

class ChatServer : public Chat
{
	TcpSocket listen_socket_;
	std::map<int, TcpSocket> clients_;
	std::map<TcpSocket, int> clients_ids_;
public:
	ChatServer(const std::string& address, int port);
	TcpSocket GetClient(int id);
	virtual void Bind(AsyncSocketHandler& handler) override;
	virtual void Broadcast(const char* buf, int len) override;
	virtual void AcceptedHandler(TcpSocket socket) override;
	virtual void ReceivedHandler(TcpSocket socket) override;
	virtual void ClosedHandler(TcpSocket socket) override;
};
