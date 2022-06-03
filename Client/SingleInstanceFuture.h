#pragma once

#include <future>
#include <chrono>
#include <functional>
#include <utility>


//A simple future wrapper class. Allows only a single instance to be active at any single time; particularly useful for GUI operations where we don't want
//to send the same commands or repeat operations before they completed. Also wraps std::async nicely with setFromAsync
template <class T>
class SingleInstanceFuture : public std::future<T>
{
public:
	SingleInstanceFuture(void) = default;
	template<class Func, class... Args>
	void setFromAsync(Func func, Args&&... args) noexcept(false);
	bool ready();

private:
	SingleInstanceFuture(std::future<T> other);
	SingleInstanceFuture<T> operator=(std::future<T>& other);
};

template<class T>
template<class Func, class ...Args>
inline void SingleInstanceFuture<T>::setFromAsync(Func func, Args && ...args)
{
	if (this->valid())
	{
		throw std::runtime_error("The asynchronous action was cancelled before it finished executing");
	}
	*this = std::async(std::launch::async, func, std::forward<Args>(args)...);
}

template<class T>
inline bool SingleInstanceFuture<T>::ready()
{
	if (!this->valid())
	{
		return false;
	}
	return this->wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}


template<class T>
inline SingleInstanceFuture<T>::SingleInstanceFuture(std::future<T> other) : std::future<T>(std::move(other)) {}

template<class T>
inline SingleInstanceFuture<T> SingleInstanceFuture<T>::operator=(std::future<T>& other)
{
	return std::future<T>::operator=(other);
}
