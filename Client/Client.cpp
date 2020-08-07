#include <stdio.h>
#include <memory>
#include "WindowsBluetoothConnector.h"
#include "CommandSerializer.h"

constexpr BTH_ADDR XM3_ADDR = 0x38184cbf447f;

using BluetoothConnectorPtr = std::shared_ptr<IBluetoothConnector>;

int main()
{
    try
    {
        BluetoothConnectorPtr connector = std::make_shared<WindowsBluetoothConnector>(XM3_ADDR);
        auto command = CommandSerializer::serializeNcAndAsmSetting(
            NC_ASM_EFFECT::ADJUSTMENT_IN_PROGRESS,
            NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
            0,
            ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
            ASM_ID::NORMAL,
            10
        );
        auto toSend = CommandSerializer::packageDataForBt(command, DATA_TYPE::DATA_MDR, 0);
        connector->send(toSend.data(), toSend.size());
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}