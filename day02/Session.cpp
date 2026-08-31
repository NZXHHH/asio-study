#include "Session.h"
void Session::Connect(asio::ip::tcp::endpoint& ep)
{
	soc->connect(ep);
}

void Session::WriteBack(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	auto& temp_node = send_que.front();
	temp_node->cur_len += bytes_transferred;
	if (bytes_transferred < temp_node->total_len)
	{
		soc->async_write_some(asio::buffer(temp_node->msg+temp_node->cur_len,temp_node->total_len-bytes_transferred), bind(&Session::ReadBack, this, std::placeholders::_1, std::placeholders::_2));
		return;
	}
	send_que.pop();
	if (send_que.empty()) {
		send_pending = false;
	}
	if (!send_que.empty()) {
		auto& send_data = send_que.front();
		this->soc->async_write_some(asio::buffer(send_data->msg + send_data->cur_len, send_data->total_len - send_data->cur_len),
			std::bind(&Session::WriteBack,
				this, std::placeholders::_1, std::placeholders::_2));
	}
}

void Session::ReadBack(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	recv_node->cur_len += bytes_transferred;
	if (recv_node->cur_len < recv_node->total_len) {
		soc->async_read_some(asio::buffer(recv_node->msg + recv_node->cur_len,
			recv_node->total_len - recv_node->cur_len), std::bind(&Session::WriteBack, this,
				std::placeholders::_1, std::placeholders::_2));
		return;
	}

	//将数据投递到队列里交给逻辑线程处理，此处略去
	//如果读完了则将标记置为false
	recv_pending = false;
	//指针置空
	recv_node = nullptr;
	ReadFromSoc();
}

void Session::WriteToSoc(std::string &buf)
{
	send_que.emplace(new MsgNode(buf.c_str(), buf.length()));
	if (send_pending) return;
	
	soc->async_write_some(asio::buffer(buf), bind(&Session::ReadBack, this, std::placeholders::_1, std::placeholders::_2));
	send_pending = true;
}

void Session::ReadFromSoc()
{
	if (recv_pending) return;
	recv_node = std::make_shared<MsgNode>(RECVSIZE);
	soc->async_read_some(asio::buffer(recv_node->msg, recv_node->total_len), bind(&Session::ReadBack, this, std::placeholders::_1, std::placeholders::_2);
	recv_pending = true;
}
