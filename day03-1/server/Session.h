#pragma once
#include "boost/asio.hpp"
#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
using namespace std;
using  boost::asio::ip::tcp;
#include <queue>
#include<map>
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
class CServer;
class CSession:public enable_shared_from_this<CSession>
{
public:
	CSession(boost::asio::io_context& ios,CServer *server) :soc(ios),server(server) {
		boost::uuids::uuid  a_uuid = boost::uuids::random_generator()();
		_uuid = boost::uuids::to_string(a_uuid);
	};
	void Start();
	void handle_write(boost::system::error_code, std::size_t,shared_ptr<CSession>);
	void handle_read(boost::system::error_code, std::size_t, shared_ptr<CSession>);
	tcp::socket& Socket() { return soc; };
	void send(char* buf, int length);
	std::string& GetUuid();
private:
	tcp::socket soc;
	char data[RECVSIZE];
	std::mutex _send_lock;
	std::queue<shared_ptr<MsgNode>> que;
	CServer* server;
	std::string _uuid;
};


class CServer
{
public:
	CServer(boost::asio::io_context& ioc, short port) :ioc(ioc), acceptor(ioc, tcp::endpoint(tcp::v4(), port)) {
		start_accept();
	};
	void start_accept();
	void handle_accept(shared_ptr<CSession>session,boost::system::error_code error);
	void ClearSession(std::string uuid);
private:
	boost::asio::io_context& ioc;
	tcp::acceptor acceptor;
	std::map<std::string, shared_ptr<CSession>> _sessions;
};