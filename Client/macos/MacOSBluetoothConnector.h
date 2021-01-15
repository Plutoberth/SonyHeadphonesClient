#pragma once
#include <stdio.h>
#include "../IBluetoothConnector.h"
#include "IOBluetooth/IOBluetooth.h"
#include "Constants.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <deque>

class MacOSBluetoothConnector final : public IBluetoothConnector
{
public:
    MacOSBluetoothConnector();
    ~MacOSBluetoothConnector();
    static void connectToMac(MacOSBluetoothConnector* MacOSBluetoothConnector) noexcept(false);
    virtual void connect(const std::string& addrStr) noexcept(false);
    virtual int send(char* buf, size_t length) noexcept(false);
    virtual int recv(char* buf, size_t length) noexcept(false);
    virtual void disconnect() noexcept;
    virtual bool isConnected() noexcept;
    virtual void closeConnection();
    
    virtual std::vector<BluetoothDevice> getConnectedDevices() noexcept(false);
    std::deque<std::vector<unsigned char>> receivedBytes;
    std::mutex receiveDataMutex;
    std::condition_variable receiveDataConditionVariable;
    std::atomic<bool> running = false;
    
private:
    void *rfcommDevice;
    void *rfcommchannel;
    std::thread* uthread = NULL;
};
