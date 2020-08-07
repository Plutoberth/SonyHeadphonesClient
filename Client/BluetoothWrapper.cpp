#include "BluetoothWrapper.h"

BluetoothWrapper::BluetoothWrapper(std::shared_ptr<IBluetoothConnector> connector)
{
    this->_connector = connector;
}

int BluetoothWrapper::sendCommand(const std::vector<char>& bytes)
{
    auto data = CommandSerializer::_packageDataForBt(bytes, DATA_TYPE::DATA_MDR, this->_seqNumber++);
    return this->_connector->send(data.data(), data.size());
}
