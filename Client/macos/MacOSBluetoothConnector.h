#pragma once
#include <stdio.h>
#include <iostream>
#include <rpc/rpc.h>
#include "../IBluetoothConnector.h"
#include "IOBluetooth/IOBluetooth.h"
#include <string>
#include "ByteMagic.h"
#include <atomic>

#include <thread>
class MacOSBluetoothConnector final : public IBluetoothConnector
{
public:
	MacOSBluetoothConnector();
	~MacOSBluetoothConnector();
	static void connectToMac(MacOSBluetoothConnector* MacOSBluetoothConnector) noexcept(false);
	virtual void dataRec(const char *dataReceived) noexcept(false);
	virtual void connect(const std::string& addrStr) noexcept(false);
	virtual int send(char* buf, size_t length) noexcept(false);
	virtual int recv(char* buf, size_t length) noexcept(false);
	virtual void disconnect() noexcept;
	virtual bool isConnected() noexcept;
	virtual void closeConnection();
	
	virtual std::vector<BluetoothDevice> getConnectedDevices() noexcept(false);

private:
    void *rfcommDevice;
    void *rfcommchannel;
    std::thread* uthread = NULL;
    int running = 0;

};