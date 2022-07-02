#include "chat_server.h"

ChatServer::ChatServer(const std::string& address, int port) : listen_socket_(true)
{
	listen_socket_.Bind(address, port);
	listen_socket_.Listen();
}

TcpSocket ChatServer::GetClient(int id)
{
	return clients_.count(id) ? clients_[id] : TcpSocket();
}

void ChatServer::Bind(AsyncSocketHandler& handler)
{
	Chat::Bind(handler);
	handler.Register(listen_socket_, FD_ACCEPT | FD_CLOSE);
}

void ChatServer::Broadcast(const char* buf, int len)
{
	AppendText((TCHAR*)buf);
	for (auto& client : clients_)
		client.second.Send(buf, len);
}

void ChatServer::AcceptedHandler(TcpSocket socket)
{
	int id = clients_.empty() ? 0 : clients_.rbegin()->first + 1;
	clients_[id] = socket;
	clients_ids_[socket] = id;
	AppendText(TEXT("Client has joined."));
}

void ChatServer::ReceivedHandler(TcpSocket socket)
{
	TCHAR buffer[1024];
	int size = socket.Receive((char*)buffer, sizeof(buffer), 0);
	Broadcast((char*)buffer, size);
}

void ChatServer::ClosedHandler(TcpSocket socket)
{
	clients_.erase(clients_ids_[socket]);
	clients_ids_.erase(socket);
	AppendText(TEXT("Client has left."));
}
