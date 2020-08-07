#pragma once

#include "IBluetoothConnector.h"
#include "CommandSerializer.h"
#include "Constants.h"
#include <memory>
#include <vector>

class BluetoothWrapper
{
public:
	BluetoothWrapper(std::shared_ptr<IBluetoothConnector> connector);
	int sendCommand(const std::vector<char>& bytes);

private:
	std::shared_ptr<IBluetoothConnector> _connector;
	unsigned int _seqNumber = 0;
};