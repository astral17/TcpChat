#include "tcp_client.h"

TcpClient::TcpClient(const std::string& address, int port) : socket_(true)
{
	socket_.Connect(address, port);
}

void TcpClient::Bind(AsyncSocketHandler& handler)
{
	Tcp::Bind(handler);
	handler.Register(socket_, FD_READ | FD_CLOSE);
}

void TcpClient::Broadcast(const char* buf, int len)
{
	socket_.Send(buf, len);
}

void TcpClient::AcceptedHandler(TcpSocket socket)
{
}

void TcpClient::ReceivedHandler(TcpSocket socket)
{
}

void TcpClient::ClosedHandler(TcpSocket socket)
{
	socket.Close();
}
