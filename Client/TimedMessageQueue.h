#pragma once

#include <vector>
#include <string>
#include <deque>
#include <chrono>
#include <utility>

using LargeCounter = unsigned long long;
using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;

struct Message
{
public:
	Message(std::string message, TimePoint releaseTime) 
	{
		this->message = std::move(message);
		this->_releaseTime = releaseTime;
	}

	friend class TimedMessageQueue;

	std::string message;
private:
	TimePoint _releaseTime;
};

class TimedMessageQueue
{
public:
	//I: A message, and the length of time it should remain available
	void addMessage(std::string message, unsigned long long lengthMs);

	//The iteators returned are guaranteed to stay valid only until the next begin() call.
	std::deque<Message>::const_iterator begin();
	std::deque<Message>::const_iterator end() const;
private:
	std::deque<Message> _messages;
	
};