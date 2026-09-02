#include "Session.h"
void CSession::Start()
{
	memset(data, 0, RECVSIZE);
	soc.async_read_some(boost::asio::buffer(data, RECVSIZE),
		bind(&CSession::handle_read, this, placeholders::_1, placeholders::_2, shared_from_this()));
}

void CSession::handle_write(boost::system::error_code error, std::size_t, shared_ptr<CSession> session)
{
	if (!error)
	{
		que.pop();
		if (!que.empty())
		{
			auto top_msg = que.front();
			soc.async_write_some(boost::asio::buffer(top_msg->msg, top_msg->total_len),
				bind(&CSession::handle_write, this, placeholders::_1, placeholders::_2, session
				));
		}
	}
	else
	{
		server->ClearSession(_uuid);
	}
	
}

void CSession::handle_read(boost::system::error_code error, std::size_t tranfered, shared_ptr<CSession> session)
{
	if (!error)
	{
		send(data,tranfered);
		memset(data, 0, RECVSIZE);
		soc.async_read_some(boost::asio::buffer(data, RECVSIZE), 
			bind(&CSession::handle_read, this, placeholders::_1, placeholders::_2, session));
	}
	else
	{
		server->ClearSession(session->GetUuid());
	}
}

void CSession::send(char* buf,int length)
{
	std::lock_guard<std::mutex> lock(_send_lock);
	que.push(make_shared<MsgNode>(buf, length));
	if (que.size() == 1)
	{
		auto first_msg = que.front();
		
		soc.async_write_some(boost::asio::buffer(first_msg->msg, first_msg->total_len),
			bind(&CSession::handle_write, this, placeholders::_1, placeholders::_2, shared_from_this()
			));
	}

}

std::string& CSession::GetUuid()
{
	return _uuid;
}

void CServer::start_accept()
{
	shared_ptr<CSession> session = make_shared<CSession>(ioc,this);
	acceptor.async_accept(session->Socket(), bind(&CServer::handle_accept, this, session,placeholders::_1));
}

void CServer::handle_accept(shared_ptr<CSession> session, boost::system::error_code error)
{
	if (!error)
	{
		session->Start();
		_sessions.insert(make_pair(session->GetUuid(), session));

	}
	start_accept();

}
void CServer::ClearSession(std::string uuid) {
	_sessions.erase(uuid);
}






