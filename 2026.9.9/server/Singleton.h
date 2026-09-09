#pragma once
#include <memory>
#include <mutex>
#include <iostream>
using namespace std;
template <typename T>
class Singleton
{
	friend class LogicSystem;
private:
	Singleton() = default;
	Singleton<T>& operator=(Singleton<T>& s) = delete;
	Singleton(Singleton<T>&) = delete;
	static std::shared_ptr<T> _instance;
public:
	static std::shared_ptr<T> GetInstance()
	{
		std::once_flag s_flag;
		std::call_once(s_flag, [&]()
			{
				//_instance = make_shared<T>();
				_instance = std::shared_ptr<T>(new T);
				return _instance;
			});
		return _instance;
	}
};
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;