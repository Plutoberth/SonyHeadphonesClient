#include "Listener.h"

Listener::Listener(Headphones& headphones, BluetoothWrapper& bt): 
_headphones(headphones),
_bt(bt)
{}

void Listener::listen()
{
    std::cout << "Listener registered." << std::endl;
    for(;;)
    {
        Buffer reply = this->_bt.readReplies();
        this->handle_message(reply);
    }
}

inline BtMessage Listener::parse(Buffer msg)
{
    return CommandSerializer::unpackBtMessage(msg);
}

void Listener::handle_message(Buffer msg)
{
    BtMessage m = this->parse(msg);
    this->_bt.setSeqNumber(m.seqNumber);

    if (m.dataType == DATA_TYPE::ACK)
    {
        this->_bt.postAck();
        this->_bt._ack.notify_all();
    } 
    else if (m.dataType == DATA_TYPE::DATA_MDR || m.dataType == DATA_TYPE::DATA_MDR_NO2) 
    {
        // Set these values as current values of Headphone property
        this->_headphones.setStateFromReply(m);
        this->_bt.sendAck();
    }
}
