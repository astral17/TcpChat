#include "tcp.h"

void Tcp::Bind(AsyncSocketHandler& handler)
{
	accepted_handler_.BindCallback([this](TcpSocket socket)
	{
		AcceptedHandler(socket);
	});
	handler.accepted.Add(accepted_handler_);

	received_handler_.BindCallback([this](TcpSocket socket)
	{
		ReceivedHandler(socket);
	});
	handler.received.Add(received_handler_);

	closed_handler_.BindCallback([this](TcpSocket socket)
	{
		ClosedHandler(socket);
	});
	handler.closed.Add(closed_handler_);
}
