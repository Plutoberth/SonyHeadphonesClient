#include "BluetoothWrapper.h"

#if (defined(_WIN32) || defined(_WIN64)) && defined(_DEBUG)
#include "windows.h"
#endif

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
	std::lock_guard guard(this->_wrapperMtx);
	auto data = CommandSerializer::_packageDataForBt(bytes, DATA_TYPE::DATA_MDR, this->_seqNumber++);
	auto bytesSent = this->_connector->send(data.data(), data.size());
	return bytesSent;
}

bool BluetoothWrapper::isConnected()
{
	std::lock_guard guard(this->_wrapperMtx);
	return this->_connector->isConnected();
}

void BluetoothWrapper::connect(const std::string& addr)
{
	std::lock_guard guard(this->_wrapperMtx);
	this->_connector->connect(addr);
}

void BluetoothWrapper::disconnect()
{
	std::lock_guard guard(this->_wrapperMtx);
	this->_seqNumber = 0;
	this->_connector->disconnect();
}


std::vector<BluetoothDevice> BluetoothWrapper::getConnectedDevices()
{
#if (defined(_WIN32) || defined(_WIN64)) && defined(_DEBUG)
	Sleep(1000);
#endif

	return this->_connector->getConnectedDevices();
}