#pragma once

#include "IBluetoothConnector.h"
#include "CommandSerializer.h"
#include "Constants.h"
#include <memory>
#include <vector>
#include <string>
#include <mutex>


//Thread-safety: This class is thread-safe.
class BluetoothWrapper
{
public:
	BluetoothWrapper(std::unique_ptr<IBluetoothConnector> connector);

	BluetoothWrapper(const BluetoothWrapper&) = delete;
	BluetoothWrapper& operator=(const BluetoothWrapper&) = delete;

	BluetoothWrapper(BluetoothWrapper&& other) noexcept;
	BluetoothWrapper& operator=(BluetoothWrapper&& other) noexcept;

	int sendCommand(const std::vector<char>& bytes);

	bool isConnected();
	//Try to connect to the headphones
	void connect(const std::string& addr);
	void disconnect();

	std::vector<BluetoothDevice> getConnectedDevices();

private:
	void _waitForAck();

	std::unique_ptr<IBluetoothConnector> _connector;
	std::mutex _wrapperMtx;
	unsigned int _seqNumber = 0;
};