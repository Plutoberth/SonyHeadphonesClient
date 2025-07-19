#pragma once

#include "IBluetoothConnector.h"
#include "CommandSerializer.h"
#include "Constants.h"
#include <memory>
#include <vector>
#include <string>
#include <mutex>
#include <future>
#include <iostream>

//Thread-safety: This class is thread-safe.
class BluetoothWrapper
{
public:
	BluetoothWrapper(std::unique_ptr<IBluetoothConnector> connector);

	BluetoothWrapper(const BluetoothWrapper&) = delete;
	BluetoothWrapper& operator=(const BluetoothWrapper&) = delete;

	BluetoothWrapper(BluetoothWrapper&& other) noexcept;
	BluetoothWrapper& operator=(BluetoothWrapper&& other) noexcept;

	int sendCommand(const std::vector<char>& bytes, DATA_TYPE dtype = DATA_TYPE::DATA_MDR);
	void sendAck(unsigned int seqNumber);

	Buffer readReplies();

	bool isConnected() noexcept;
	//Try to connect to the headphones
	void connect(const std::string& addr);
	void disconnect() noexcept;

	std::vector<BluetoothDevice> getConnectedDevices();
	void setSeqNumber(unsigned char seqNumber);
	void postAck();

private:
	void _waitForAck();

	std::unique_ptr<IBluetoothConnector> _connector;
	std::mutex _connectorMtx;
	std::mutex _dataMtx;
	
	/*
		seqNumber logic:
			every command that client sends has the inverse seqNumber of the last ACK packet sent by the headphones
			every ACK packet sent by the client has the inverse seqNumber as the response being ACK'd (this is passed as a parameter to sendACK)
			both seqNumbers are independent of each other
	*/
	unsigned char _seqNumber = 0;
	unsigned int _ackBuffer = 0;

public:
	std::condition_variable _ack;
};
