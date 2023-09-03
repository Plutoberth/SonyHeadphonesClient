#pragma once

#include "Constants.h"
#include "BluetoothWrapper.h"
#include "CommandSerializer.h"
#include "Exceptions.h"
#include "Headphones.h"

#include <future>

class Listener
{
    /*
        Listen for and read incoming messages
        Parse messages 
        Call message handler
    */
    /* 
        have a valid bit for ack messages 
            - when command is sent, it is set to 0 by locking
            - when ack is received, set it to 1
    */
public:
    Listener(Headphones& headphones, BluetoothWrapper& bt);
    void listen();
    inline BtMessage parse(Buffer msg);
    void handle_message(Buffer msg);
    bool getAck();

private:

    Headphones& _headphones;
    BluetoothWrapper& _bt;

    std::mutex _listenerMtx;
    std::condition_variable _condt;
    bool _ackRecvd;

};