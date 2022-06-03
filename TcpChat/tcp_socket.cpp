#include "tcp_socket.h"
#include <assert.h>

WSAData WsaHolder::data_;
int WsaHolder::counter = 0;

WsaHolder::WsaHolder()
{
	if (!counter++)
		assert(!WSAStartup(MAKEWORD(2, 2), &data_));
}

WsaHolder::~WsaHolder()
{
	if (!--counter)
		WSACleanup();
}

WSAData WsaHolder::data() const
{
	return data_;
}

WsaHolder TcpSocket::wsa;

TcpSocket::TcpSocket()
{
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

TcpSocket::TcpSocket(SOCKET socket) : socket_(socket)
{
}

TcpSocket::~TcpSocket()
{
	if (socket_ != INVALID_SOCKET)
		closesocket(socket_);
}

bool TcpSocket::Bind(const std::string& address, int port)
{
	sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = inet_addr(address.c_str());
	local.sin_port = htons(port);
	memset(&local.sin_zero, 0, sizeof(local.sin_zero));
	return !bind(socket_, (sockaddr*)&local, sizeof(local));
}

bool TcpSocket::Listen(int max_queue)
{
	return !listen(socket_, max_queue);
}

TcpSocket TcpSocket::Accept()
{
	sockaddr_in from;
	int len = sizeof(from);
	return TcpSocket(accept(socket_, (sockaddr*)&from, &len));
}

bool TcpSocket::Connect(const std::string& address, int port)
{
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(address.c_str());
	server.sin_port = htons(port);
	memset(&server.sin_zero, 0, sizeof(server.sin_zero));
	return !connect(socket_, (sockaddr*)&server, sizeof(server));
}

void TcpSocket::Send(const char* buf, int len, int flags)
{
	send(socket_, buf, len, flags);
}

int TcpSocket::Receive(char* buf, int len, int flags)
{
	return recv(socket_, buf, len, flags);
}

bool TcpSocket::IsValid() const
{
	return socket_ != INVALID_SOCKET;
}

SOCKET TcpSocket::RawSocket() const
{
	return socket_;
}
