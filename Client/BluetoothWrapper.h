#pragma once

#include "Listener.h"
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
	BluetoothWrapper(Listener * Listener, std::unique_ptr<IBluetoothConnector> connector);

	BluetoothWrapper(const BluetoothWrapper&) = delete;
	BluetoothWrapper& operator=(const BluetoothWrapper&) = delete;

	BluetoothWrapper(BluetoothWrapper&& other) noexcept;
	BluetoothWrapper& operator=(BluetoothWrapper&& other) noexcept;

	int sendCommand(const std::vector<char>& bytes);
	int sendCommand(const std::vector<char>& bytes, DATA_TYPE dtype);

	Buffer readReplies();

	bool isConnected() noexcept;
	//Try to connect to the headphones
	void connect(const std::string& addr);
	void disconnect() noexcept;

	std::vector<BluetoothDevice> getConnectedDevices();
	void setSeqNumber(unsigned int seqNumber);

private:
	void _waitForAck();

	Listener * _listener;
	std::unique_ptr<IBluetoothConnector> _connector;
	std::mutex _connectorMtx;
	unsigned int _seqNumber = 0;
};
