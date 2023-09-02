#include "Listener.h"

// Listener::Listener()//: 
// // _ackRecvd(false)
// {

// }

void Listener::listen()
{
    // do
    for(;;)
    {
        Buffer reply = _bt.readReplies();
        this->handle_message(reply);
    }// while(1);
}

inline BtMessage Listener::parse(Buffer msg)
{
    return CommandSerializer::unpackBtMessage(msg);
}

void Listener::handle_message(Buffer msg)
{
    BtMessage m = this->parse(msg);
    if (m.dataType == DATA_TYPE::ACK)
    {
        // std::condition_variable::notify_all
        std::lock_guard guard(this->_listenerMtx);
        this->_ackRecvd = true;
    } else if (m.dataType == DATA_TYPE::DATA_MDR || m.dataType == DATA_TYPE::DATA_MDR_NO2) {
        // Set these values as current values of Headphone property
        // Send ACK message to real headphone to shut it up!
        this->_bt.sendCommand({}, DATA_TYPE::ACK);
    }
}
