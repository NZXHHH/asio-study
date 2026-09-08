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
#include"const.h"
class MsgNode
{
public:
	MsgNode(const char* buf, int total_len) :total_len(total_len+HEAD_DATA_LEN), cur_len(0)
	{
		_data = new char[total_len + 1];
		memcpy(_data, &total_len, HEAD_DATA_LEN);
		memcpy(_data+HEAD_DATA_LEN, buf, total_len);
		_data = new char[total_len];
		memcpy(_data, buf, total_len);
		_data[total_len] = '\0';
	}
	MsgNode(int total_len) :total_len(total_len), cur_len(0)
	{
		_data = new char[total_len+1];
		_data[total_len] = '\0';
	}
	char* _data;
	int total_len;
	int cur_len;
};