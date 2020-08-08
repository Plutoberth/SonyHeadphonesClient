#pragma once

#include "IBluetoothConnector.h"
#include "CommandSerializer.h"
#include "Constants.h"
#include <memory>
#include <vector>
#include <string>

class BluetoothWrapper
{
public:
	BluetoothWrapper(std::unique_ptr<IBluetoothConnector> connector);

	int sendCommand(const std::vector<char>& bytes);
	//Try to connect to the headphones
	void connect(const std::string& addr);
	void disconnect();

	BluetoothWrapper(const BluetoothWrapper&) = delete;
	BluetoothWrapper(BluetoothWrapper&&) = delete;

	BluetoothWrapper& operator=(const BluetoothWrapper&) = delete;
	BluetoothWrapper& operator=(BluetoothWrapper&&) = delete;

private:
	std::unique_ptr<IBluetoothConnector> _connector;
	unsigned int _seqNumber = 0;
};