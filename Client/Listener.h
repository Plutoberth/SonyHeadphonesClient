#pragma once

#include "imgui/imgui.h"
#include "Constants.h"
#include "IBluetoothConnector.h"
#include "BluetoothWrapper.h"
#include "CommandSerializer.h"
#include "Exceptions.h"
#include "TimedMessageQueue.h"
#include "SingleInstanceFuture.h"
#include "CascadiaCodeFont.h"
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
    Listener();
    void listen();
    inline BtMessage parse(Buffer msg);
    void handle_message(Buffer msg);

private:

    Headphones& _headphones;
    BluetoothWrapper& _bt;

    std::mutex _listenerMtx;
    bool _ackRecvd;

};