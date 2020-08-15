#pragma once
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "Bthprops.lib")
#include <winsock2.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>
#include <iostream>
#include <rpc.h>
#include "IBluetoothConnector.h"
#include <string>
#include "ByteMagic.h"

class WindowsBluetoothConnector final : public IBluetoothConnector
{
public:
	WindowsBluetoothConnector();

	~WindowsBluetoothConnector();

	virtual void connect(const std::string& addrStr) noexcept(false);
	virtual int send(char* buf, size_t length) noexcept(false);
	virtual void disconnect() noexcept(false);
	virtual bool isConnected() noexcept;

	virtual std::vector<BluetoothDevice> getConnectedDevices() noexcept(false);

private:
	std::vector<BluetoothDevice> findDevicesInRadio(BLUETOOTH_DEVICE_SEARCH_PARAMS* search_params);

	SOCKET _socket = INVALID_SOCKET;
	bool _connected = false;
	void _initSocket();
};