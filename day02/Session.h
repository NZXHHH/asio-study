#pragma once
#include "boost/asio.hpp"
#include <iostream>
using namespace std;
using namespace boost;
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
	Session(std::shared_ptr<asio::ip::tcp::socket> soc) :soc(soc), recv_pending(false), send_pending(false) {};
	void Connect(asio::ip::tcp::endpoint& ep);
	void WriteBack(const boost::system::error_code& ec, std::size_t bytes_transferred);
	void ReadBack(const boost::system::error_code& ec, std::size_t bytes_transferred);
	void WriteToSoc(std::string& buf);
	void ReadFromSoc();
private:
	bool recv_pending;
	bool send_pending;
	queue<std::shared_ptr<MsgNode>>send_que;
	std::shared_ptr<asio::ip::tcp::socket> soc;
	std::shared_ptr<MsgNode>recv_node;
	std::shared_ptr<MsgNode>send_node;
};