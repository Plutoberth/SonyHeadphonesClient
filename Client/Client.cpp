#include <stdio.h>
#include <memory>
#include "WindowsBluetoothConnector.h"
#include "CommandSerializer.h"
#include "BluetoothWrapper.h"
#include "imgui/imgui.h"

#include "GUI_Impls/WindowsGUI.h"

using BluetoothConnectorPtr = std::shared_ptr<IBluetoothConnector>;

constexpr BTH_ADDR XM3_ADDR = 0x38184cbf447f;

int main()
{
    EnterGUIMainLoop();

    try
    {
        BluetoothConnectorPtr connector = std::make_shared<WindowsBluetoothConnector>(XM3_ADDR);
        BluetoothWrapper wrap(connector);
        auto command = CommandSerializer::serializeNcAndAsmSetting(
            NC_ASM_EFFECT::ON,
            NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
            0,
            ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
            ASM_ID::NORMAL,
            0
        );
        
        wrap.sendCommand(command);

        command = CommandSerializer::serializeNcAndAsmSetting(
            NC_ASM_EFFECT::ADJUSTMENT_COMPLETION,
            NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
            0,
            ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
            ASM_ID::VOICE,
            1
        );

        wrap.sendCommand(command);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

