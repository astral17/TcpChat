#include "tcp_server.h"

TcpServer::TcpServer(const std::string& address, int port) : listen_socket_(true)
{
	listen_socket_.Bind(address, port);
	listen_socket_.Listen();
}

TcpSocket TcpServer::GetClient(int id)
{
	return clients_.count(id) ? clients_[id] : TcpSocket();
}

void TcpServer::Bind(AsyncSocketHandler& handler)
{
	Tcp::Bind(handler);
	handler.Register(listen_socket_, FD_ACCEPT | FD_CLOSE);
}

void TcpServer::Broadcast(const char* buf, int len)
{
	for (auto& client : clients_)
		client.second.Send(buf, len);
}

void TcpServer::AcceptedHandler(TcpSocket socket)
{
	int id = clients_.rbegin()->first + 1;
	clients_[id] = socket;
	clients_ids_[socket] = id;
}

void TcpServer::ReceivedHandler(TcpSocket socket)
{
}

void TcpServer::ClosedHandler(TcpSocket socket)
{
	clients_.erase(clients_ids_[socket]);
	clients_ids_.erase(socket);
}
