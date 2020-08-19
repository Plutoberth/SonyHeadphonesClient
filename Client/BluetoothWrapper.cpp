#include "BluetoothWrapper.h"

BluetoothWrapper::BluetoothWrapper(std::unique_ptr<IBluetoothConnector> connector)
{
	this->_connector.swap(connector);
}

BluetoothWrapper::BluetoothWrapper(BluetoothWrapper&& other) noexcept
{
	this->_connector.swap(other._connector);
	this->_seqNumber = other._seqNumber;
}

BluetoothWrapper& BluetoothWrapper::operator=(BluetoothWrapper&& other) noexcept
{
	//self assignment
	if (this == &other) return *this;

	this->_connector.swap(other._connector);
	this->_seqNumber = other._seqNumber;

	return *this;
}

int BluetoothWrapper::sendCommand(const std::vector<char>& bytes)
{
	
	auto data = CommandSerializer::packageDataForBt(bytes, DATA_TYPE::DATA_MDR, this->_seqNumber++);
	int bytesSent = 0;
	try
	{
		std::lock_guard guard(this->_connectorMtx);
		bytesSent = this->_connector->send(data.data(), data.size());
		this->_waitForAck();
	}
	catch (const RecoverableException& e)
	{
		if (e.shouldDisconnect)
		{
			this->disconnect();
		}
		throw;
	}
	
	return bytesSent;
}

bool BluetoothWrapper::isConnected() noexcept
{
	
	try
	{
		std::lock_guard guard(this->_connectorMtx);
		return this->_connector->isConnected();
	}
	catch (const RecoverableException& e)
	{
		if (e.shouldDisconnect)
		{
			this->disconnect();
		}
		throw;
	}
}

void BluetoothWrapper::connect(const std::string& addr)
{
	try
	{
		std::lock_guard guard(this->_connectorMtx);
		return this->_connector->connect(addr);
	}
	catch (const RecoverableException& e)
	{
		if (e.shouldDisconnect)
		{
			this->disconnect();
		}
		throw;
	}
}

void BluetoothWrapper::disconnect() noexcept
{
	
	this->_seqNumber = 0;
	try
	{
		std::lock_guard guard(this->_connectorMtx);
		return this->_connector->disconnect();
	}
	catch (const RecoverableException& e)
	{
		if (e.shouldDisconnect)
		{
			this->disconnect();
		}
		throw;
	}
	
}


std::vector<BluetoothDevice> BluetoothWrapper::getConnectedDevices()
{
	try
	{
		std::lock_guard guard(this->_connectorMtx);
		return this->_connector->getConnectedDevices();
	}
	catch (const RecoverableException& e)
	{
		if (e.shouldDisconnect)
		{
			this->disconnect();
		}
		throw;
	}
}

void BluetoothWrapper::_waitForAck()
{
	bool ongoingMessage = false;
	bool messageFinished = false;
	char buf[MAX_BLUETOOTH_MESSAGE_SIZE] = { 0 };
	Buffer msgBytes;

	do
	{
		auto numRecvd = this->_connector->recv(buf, sizeof(buf));
		size_t messageStart = 0;
		size_t messageEnd = numRecvd;

		for (size_t i = 0; i < numRecvd; i++)
		{
			if (buf[i] == START_MARKER)
			{
				if (ongoingMessage)
				{
					throw RecoverableException("Invalid: Multiple start markers without an end marker", true);
				}
				messageStart = i + 1;
				ongoingMessage = true;
			}
			else if (ongoingMessage && buf[i] == END_MARKER)
			{
				messageEnd = i;
				ongoingMessage = false;
				messageFinished = true;
			}
		}

		msgBytes.insert(msgBytes.end(), buf + messageStart, buf + messageEnd);
	} while (!messageFinished);

	auto msg = CommandSerializer::unpackBtMessage(msgBytes);
	this->_seqNumber = msg.seqNumber;
}

