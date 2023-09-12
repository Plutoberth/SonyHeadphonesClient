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

int BluetoothWrapper::sendCommand(const std::vector<char>& bytes, DATA_TYPE dtype)
{
	int bytesSent;
	std::lock_guard guard(this->_connectorMtx);
	auto data = CommandSerializer::packageDataForBt(bytes, dtype, this->_seqNumber ^ 0x01);
	bytesSent = this->_connector->send(data.data(), data.size());

	if (dtype != DATA_TYPE::ACK)
		this->_waitForAck();

	return bytesSent;
}

void BluetoothWrapper::sendAck()
{
	auto data = CommandSerializer::packageDataForBt({}, DATA_TYPE::ACK, this->_seqNumber ^ 0x01);
	this->_connector->send(data.data(), data.size());
}

bool BluetoothWrapper::isConnected() noexcept
{
	return this->_connector->isConnected();
}

void BluetoothWrapper::connect(const std::string& addr)
{
	std::lock_guard guard(this->_connectorMtx);
	this->_connector->connect(addr);
}

void BluetoothWrapper::disconnect() noexcept
{
	std::lock_guard guard(this->_connectorMtx);
	this->_seqNumber = 0;
	this->_connector->disconnect();
}

std::vector<BluetoothDevice> BluetoothWrapper::getConnectedDevices()
{
	return this->_connector->getConnectedDevices();
}

void BluetoothWrapper::_waitForAck()
{
	std::unique_lock<std::mutex> guard(this->_dataMtx);

	while (!(this->_ackBuffer > 0)){
		this->_ack.wait(guard);
	}

	this->_ackBuffer--;
}

void BluetoothWrapper::postAck()
{
	std::lock_guard guard(this->_dataMtx);
	this->_ackBuffer++;
}

Buffer BluetoothWrapper::readReplies()
{
	bool ongoingMessage = false;
	bool messageFinished = false;
	char buf[MAX_BLUETOOTH_MESSAGE_SIZE] = { 0 };
	Buffer msgBytes;

	do
	{
		int i = 0;
		while(!messageFinished)
		{
			this->_connector->recv(buf+i, 1);
			i++;
			if (buf[i-1] == 0x3c) 
				messageFinished = true;
				// break;
		}
		auto numRecvd = i;
		size_t messageStart = 0;
		size_t messageEnd = numRecvd;
		// for (int i = 0; i < numRecvd; i++)
		// {
		// 	std::cout << std::hex << (0xff & (unsigned int)buf[i]) << " ";
		// }
		// std::cout << std::endl;

		for (int i = 0; i < numRecvd; i++)
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

	return msgBytes;

	auto msg = CommandSerializer::unpackBtMessage(msgBytes);
	this->_seqNumber = msg.seqNumber;
}

void BluetoothWrapper::setSeqNumber(unsigned char seqNumber)
{
	std::lock_guard guard(this->_dataMtx);
	this->_seqNumber = seqNumber;
}
