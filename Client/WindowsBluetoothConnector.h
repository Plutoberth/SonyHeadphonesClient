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

class WindowsBluetoothConnector : public IBluetoothConnector
{
public:
    WindowsBluetoothConnector(BTH_ADDR addr);
    ~WindowsBluetoothConnector();

    virtual int send(char* buf, size_t length);

private:
    SOCKET _socket;
};