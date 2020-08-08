#include <stdio.h>
#include <memory>
#include "WindowsBluetoothConnector.h"
#include "CommandSerializer.h"
#include "BluetoothWrapper.h"
#include "imgui/imgui.h"

#include "GUI_Impls/WindowsGUI.h"

using BluetoothConnectorPtr = std::unique_ptr<IBluetoothConnector>;

constexpr auto MY_XM3_ADDR = "38:18:4c:bf:44:7f";

int main()
{
    std::cout << "Initializing... If can't see the GUI, something has gone wrong." << std::endl;
    try
    {
        BluetoothConnectorPtr connector = std::make_unique<WindowsBluetoothConnector>();
        BluetoothWrapper wrap(std::move(connector));
        EnterGUIMainLoop(wrap);
    }
    catch (const std::exception& e)
    {
        DisplayErrorMessagebox(e.what());
    }


    try
    {
        BluetoothConnectorPtr connector = std::make_unique<WindowsBluetoothConnector>();
        BluetoothWrapper wrap(std::move(connector));
        wrap.connect(MY_XM3_ADDR);

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
        
    }
}

