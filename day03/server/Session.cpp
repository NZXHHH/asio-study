#include "Session.h"

void Session::Start()
{
	memset(data, 0, RECVSIZE);
	soc.async_read_some(boost::asio::buffer(data, RECVSIZE), bind(&Session::handle_read, this, placeholders::_1, placeholders:: _2));
}

void Session::handle_write(boost::system::error_code error, std::size_t tranfsered)
{
	if (!error)
	{
		soc.async_write_some(boost::asio::buffer(data, RECVSIZE), bind(&Session::handle_read, this, placeholders::_1, placeholders::_2));
	}
	else
		delete this;
}

void Session::handle_read(boost::system::error_code error, std::size_t tranfsered)
{
	if (!error)
	{
		soc.async_write_some(boost::asio::buffer(data), bind(&Session::handle_write, this, placeholders::_1, placeholders::_2));
	}
	else
		delete this;
	
}

void Server::start_accept()
{
	Session* session = new Session(ioc);
	acceptor.async_accept(session->Socket(), bind(&Server::handle_accept, this, session,placeholders::_1));
}

void Server::handle_accept(Session *session,boost::system::error_code error)
{
	if (!error)
	{
		session->Start();
	}
	else
		delete session;
}
