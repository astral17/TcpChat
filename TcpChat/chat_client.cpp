#include "chat_client.h"

ChatClient::ChatClient(const std::string& address, int port) : socket_(true)
{
	socket_.Connect(address, port);
}

void ChatClient::Bind(AsyncSocketHandler& handler)
{
	Chat::Bind(handler);
	handler.Register(socket_, FD_READ | FD_CLOSE);
}

void ChatClient::Broadcast(const char* buf, int len)
{
	socket_.Send(buf, len);
}

void ChatClient::AcceptedHandler(TcpSocket socket)
{
}

void ChatClient::ReceivedHandler(TcpSocket socket)
{
	TCHAR buffer[1024];
	socket.Receive((char*)buffer, sizeof(buffer), 0);
	AppendText(buffer);
}

void ChatClient::ClosedHandler(TcpSocket socket)
{
	socket.Close();
	AppendText(TEXT("Disconnected."));
}
