#include "tcp_socket.h"
#include <assert.h>

WSAData WsaHolder::data_;
int WsaHolder::counter_ = 0;

WsaHolder::WsaHolder()
{
	if (!counter_++)
		assert(!WSAStartup(MAKEWORD(2, 2), &data_));
}

WsaHolder::~WsaHolder()
{
	if (!--counter_)
		WSACleanup();
}

WSAData WsaHolder::data() const
{
	return data_;
}

WsaHolder TcpSocket::wsa_;

TcpSocket::TcpSocket(bool init) : socket_(INVALID_SOCKET)
{
	if (init)
		socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

TcpSocket::TcpSocket(const TcpSocket& other) : socket_(other.socket_)
{
}

TcpSocket::TcpSocket(SOCKET socket) : socket_(socket)
{
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

bool TcpSocket::Close()
{
	return socket_ != INVALID_SOCKET && !closesocket(socket_);
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

AsyncSocketHandler::AsyncSocketHandler(HWND hWnd, UINT message) : hWnd_(hWnd), message_(message)
{
}

void AsyncSocketHandler::Register(const TcpSocket& socket, long events)
{
	WSAAsyncSelect(socket.RawSocket(), hWnd_, message_, events);
}

void AsyncSocketHandler::OnEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (hWnd == hWnd_ && message == message_)
	{
        if (WSAGETSELECTERROR(lParam))
        {
            closesocket((SOCKET)wParam);
            return;
        }

        switch (WSAGETSELECTEVENT(lParam))
        {
        case FD_ACCEPT:
        {
            SOCKET accept_socket = accept(wParam, NULL, NULL);
			Register(accept_socket, FD_READ | FD_CLOSE);
			if (on_accept)
				on_accept(accept_socket);
        }
        break;
        case FD_READ:
        {
			if (on_message)
				on_message(wParam);
        }
        break;
        case FD_CLOSE:
			if (on_close)
				on_close(wParam);
            closesocket((SOCKET)wParam);
            break;
        }
	}
}
