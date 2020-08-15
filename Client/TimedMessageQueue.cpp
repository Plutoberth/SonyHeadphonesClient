#include "TimedMessageQueue.h"

TimedMessageQueue::TimedMessageQueue(unsigned int maxMessages, unsigned long long durationMs)
{
	this->_maxMessages = maxMessages;
	this->_durationMs = durationMs;
}

void TimedMessageQueue::addMessage(std::string message)
{
	if (this->_messages.size() == this->_maxMessages)
	{
		this->_messages.pop_front();
	}

	this->_messages.emplace_back(std::move(message), Clock::now() + std::chrono::milliseconds(this->_durationMs));
}

std::deque<Message>::const_iterator TimedMessageQueue::begin()
{
	while (this->_messages.size() > 0)
	{
		if (this->_messages.front()._releaseTime < Clock::now())
		{
			this->_messages.pop_front();
		}
		else 
		{
			//break if we reached a non-stale message, to improve perf
			break;
		}
	}

	return this->_messages.cbegin();
}

std::deque<Message>::const_iterator TimedMessageQueue::end() const
{
	return this->_messages.cend();
}
