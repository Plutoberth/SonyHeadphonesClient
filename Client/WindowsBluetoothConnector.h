#pragma once
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "rpcrt4.lib")
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
    WindowsBluetoothConnector() noexcept(false);

    ~WindowsBluetoothConnector();

    virtual void connect(const std::string& addrStr) noexcept(false);
    virtual int send(char* buf, size_t length) noexcept(false);

private:

    SOCKET _socket;
};