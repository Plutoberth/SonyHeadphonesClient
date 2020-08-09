#include "TimedMessageQueue.h"

void TimedMessageQueue::addMessage(std::string message, unsigned long long lengthMs)
{
	this->_messages.emplace_back(std::move(message), Clock::now() + std::chrono::milliseconds(lengthMs));
}

std::deque<Message>::const_iterator TimedMessageQueue::begin()
{
	while (this->_messages.size() > 0)
	{
		if (this->_messages.front()._releaseTime < Clock::now())
		{
			this->_messages.pop_front();
		}
	}

	return this->_messages.cbegin();
}

std::deque<Message>::const_iterator TimedMessageQueue::end() const
{
	return this->_messages.cend();
}
