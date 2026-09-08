#include "Session.h"
void CSession::Start()
{
	
	boost::asio::async_read(_soc,boost::asio::buffer(_head_node->_data, HEAD_DATA_LEN), bind(&CSession::handle_read, this, placeholders::_1, placeholders::_2, shared_from_this()));
	
}

void CSession::handle_write(boost::system::error_code error, std::size_t, shared_ptr<CSession> session)
{
	if (!error)
	{
		std::lock_guard<std::mutex> lock(_send_lock);
		que.pop();
		if (!que.empty())
		{
			auto top_msg = que.front();
			boost::asio::async_write(_soc,boost::asio::buffer(top_msg->_data, top_msg->total_len),
				bind(&CSession::handle_write, this, placeholders::_1, placeholders::_2, session
				));
		}
	}
	else
	{
		_server->ClearSession(_uuid);
	}
	
}

void CSession::handle_read(boost::system::error_code error, std::size_t tranfered, shared_ptr<CSession> session)
{
	if (!error)
	{
		short body_len;
		memcpy(&body_len, _head_node->_data, HEAD_DATA_LEN);
		if (body_len > MAX_LENGTH)
			_server->ClearSession(session->GetUuid());
		_recv_msg_node = make_shared<MsgNode>(body_len);
		boost::asio::async_read(_soc, boost::asio::buffer(_recv_msg_node->_data, body_len), bind(&CSession::HandleReadMsg, this, placeholders::_1, placeholders::_2, shared_from_this()));
	
	}
	else
	{
		_server->ClearSession(session->GetUuid());
	}
}

void CSession::HandleReadMsg(boost::system::error_code error, std::size_t tranfered, shared_ptr<CSession> session)
{
	if (!error)
	{
		send(_recv_msg_node->_data, _recv_msg_node->total_len);
		boost::asio::async_read(_soc, boost::asio::buffer(_head_node->_data, HEAD_DATA_LEN), bind(&CSession::handle_read, this, placeholders::_1, placeholders::_2, shared_from_this()));

	}
	else {
		_server->ClearSession(_uuid);
	}
}

void CSession::send(char* buf,int length)
{
	std::lock_guard<std::mutex> lock(_send_lock);
	que.push(make_shared<MsgNode>(buf, length));
	if (que.size() == 1)
	{
		auto first_msg = que.front();
		boost::asio::async_write(_soc,boost::asio::buffer(first_msg->_data, first_msg->total_len),
			bind(&CSession::handle_write, this, placeholders::_1, placeholders::_2, shared_from_this()
			));
	}

}

void CSession::Clear()
{
}

std::string& CSession::GetUuid()
{
	return _uuid;
}








