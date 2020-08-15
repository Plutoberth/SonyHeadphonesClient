#pragma once

#include <vector>
#include <string>
#include <deque>
#include <chrono>
#include <utility>
#include <limits>

using LargeCounter = unsigned long long;
using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;

auto constexpr DEFAULT_MAX_MESSAGES = 13371337;
constexpr auto DEFAULT_ERROR_MESSAGE_MS = 10000;

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
	TimedMessageQueue(unsigned int maxMessages = DEFAULT_MAX_MESSAGES, unsigned long long durationMs = DEFAULT_ERROR_MESSAGE_MS);

	//I: A message, and the length of time it should remain available
	void addMessage(std::string message);

	//The iteators returned are guaranteed to stay valid only until the next `begin()` or `addMessage` call.
	std::deque<Message>::const_iterator begin();
	std::deque<Message>::const_iterator end() const;
private:
	std::deque<Message> _messages;
	unsigned int _maxMessages = DEFAULT_MAX_MESSAGES;
	unsigned long long _durationMs;
};