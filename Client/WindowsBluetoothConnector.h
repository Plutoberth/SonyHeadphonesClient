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
#include <atomic>

class WindowsBluetoothConnector final : public IBluetoothConnector
{
public:
	WindowsBluetoothConnector();

	~WindowsBluetoothConnector();

	virtual void connect(const std::string& addrStr) noexcept(false);
	virtual int send(char* buf, size_t length) noexcept(false);
	virtual int recv(char* buf, size_t length) noexcept(false);
	virtual void disconnect() noexcept;
	virtual bool isConnected() noexcept;

	virtual std::vector<BluetoothDevice> getConnectedDevices() noexcept(false);

private:
	std::vector<BluetoothDevice> _findDevicesInRadio(BLUETOOTH_DEVICE_SEARCH_PARAMS* searchParams);
	std::string _wstringToUtf8(const std::wstring& wstr);

	SOCKET _socket = INVALID_SOCKET;
	std::atomic<bool> _connected = false;
	void _initSocket();
};