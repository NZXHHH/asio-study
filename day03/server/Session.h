#pragma once
#include "boost/asio.hpp"
#include <iostream>
using namespace std;
using  boost::asio::ip::tcp;
#include <queue>
//最大报文接收大小
const int RECVSIZE = 1024;
class MsgNode
{
public:
	MsgNode(const char* buf, int total_len) :total_len(total_len), cur_len(0)
	{
		msg = new char[total_len];
		memcpy(msg, buf, total_len);
	}
	MsgNode(int total_len) :total_len(total_len), cur_len(0)
	{
		msg = new char[total_len];
	}
	char* msg;
	int total_len;
	int cur_len;
};
class Session
{
public:
	Session(boost::asio::io_context& ios) :soc(ios) {};
	void Start();
	void handle_write(boost::system::error_code, std::size_t);
	void handle_read(boost::system::error_code, std::size_t);
	tcp::socket& Socket() { return soc; };
private:
	tcp::socket soc;
	char data[RECVSIZE];
};
class Server
{
public:
	Server(boost::asio::io_context& ioc, short port) :ioc(ioc), acceptor(ioc, tcp::endpoint(tcp::v4(), port)) {
		start_accept();
	};
	void start_accept();
	void handle_accept(Session* session,boost::system::error_code error);
private:
	boost::asio::io_context& ioc;
	tcp::acceptor acceptor;
};