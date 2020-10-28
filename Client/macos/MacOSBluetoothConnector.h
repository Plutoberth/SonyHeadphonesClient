#pragma once
#include <stdio.h>
#include <iostream>
#include <rpc/rpc.h>
#include "../IBluetoothConnector.h"
#include "IOBluetooth/IOBluetooth.h"
#include <string>
#include "ByteMagic.h"
#include <atomic>
class MacOSBluetoothConnector final : public IBluetoothConnector
{
public:
	MacOSBluetoothConnector();

	~MacOSBluetoothConnector();

	virtual void connect(const std::string& addrStr) noexcept(false);
	virtual int send(char* buf, size_t length) noexcept(false);
	virtual int recv(char* buf, size_t length) noexcept(false);
	virtual void disconnect() noexcept;
	virtual bool isConnected() noexcept;

	virtual std::vector<BluetoothDevice> getConnectedDevices() noexcept(false);

private:
	std::vector<BluetoothDevice> _findDevicesInRadio();
	std::string _wstringToUtf8(const std::wstring& wstr);

	// SOCKET _socket = INVALID_SOCKET;
	std::atomic<bool> _connected = false;
	void _initSocket();
};