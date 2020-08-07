#include <stdio.h>
#include <memory>
#include "WindowsBluetoothConnector.h"

constexpr BTH_ADDR XM3_ADDR = 0x38184cbf447f;

using BluetoothConnectorPtr = std::shared_ptr<IBluetoothConnector>;

int main()
{
    try
    {
        BluetoothConnectorPtr connector = std::make_shared<WindowsBluetoothConnector>(XM3_ADDR);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}