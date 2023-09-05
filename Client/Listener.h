#pragma once

#include "Constants.h"
#include "BluetoothWrapper.h"
#include "CommandSerializer.h"
#include "Exceptions.h"
#include "Headphones.h"

#include <mutex>
#include <future>

class Listener
{
    /*
        Listen for and read incoming messages
        Parse messages 
        Call message handler
    */
public:
    Listener(Headphones& headphones, BluetoothWrapper& bt);
    void listen();
    inline BtMessage parse(Buffer msg);
    void handle_message(Buffer msg);

private:
    Headphones& _headphones;
    BluetoothWrapper& _bt;
};