#include "CrossPlatformGUI.h"

void CrossPlatformGUI::performGUIPass(BluetoothWrapper& bt)
{
    ImGui::NewFrame();

    static bool show_another_window = false;
    
    
    static bool focusOnVoice = false;
    static bool sentFocusOnVoice = focusOnVoice;
    static int asmLevel = 0;
    static int sentAsmLevel = asmLevel;
    static char MAC[MAC_ADDR_STR_SIZE + 1] = "38:18:4c:bf:44:7f";
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static bool isConnected = false;
    static std::vector<BluetoothDevice> connectedDevices {{"WH-1000-XM3", "38:18:4c:bf:44:7f"},
    {"WH-1000-XM4", "38:18:4c:bf:44:7f"}, 
    {"WH-1000-XM5", "38:18:4c:bf:44:7f"} };

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        {
            static int selectedDevice = -1;
            ImGui::Begin("Device Discovery");
            ImGui::Text("Select from one of the available devices: ");

            int temp = 0;
            for (auto device: connectedDevices)
            {
                auto name = device.name + " (" + device.mac + ")";
                ImGui::RadioButton(name.c_str(), &selectedDevice, temp++);
            }

            ImGui::Spacing();

            if (ImGui::Button("Connect"))
            {
                if (selectedDevice != -1)
                {
                    ImGui::GetFrameHeightWithSpacing();
                    //TODO: This isn't proper at all. Call may block
                    bt.connect(connectedDevices[selectedDevice].mac);
                    isConnected = true;
                    ImGui::SetWindowCollapsed(true);
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Refresh devices"))
            {
                //refresh connectedDevices
            }

            ImGui::End();
        }

        if (isConnected)
        {
            ImGui::Begin("Ambient Sound Mode");                          // Create a window called "Hello, world!" and append into it.
            ImGui::Text("Control ambient sound for your %ss", "WH-1000-XM3");

            ImGui::SliderInt("ASM Level", &asmLevel, 0, 19);
            ImGui::Checkbox("Focus on Voice", &focusOnVoice);

            if (sentAsmLevel != asmLevel || sentFocusOnVoice != focusOnVoice)
            {
                bt.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
                    NC_ASM_EFFECT::ADJUSTMENT_IN_PROGRESS,
                    NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                    0,
                    ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                    focusOnVoice ? ASM_ID::VOICE : ASM_ID::NORMAL,
                    asmLevel
                ));
                bt.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
                    NC_ASM_EFFECT::ADJUSTMENT_COMPLETION,
                    NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                    0,
                    ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                    focusOnVoice ? ASM_ID::VOICE : ASM_ID::NORMAL,
                    asmLevel
                ));
                sentAsmLevel = asmLevel;
                sentFocusOnVoice = focusOnVoice;
                
            }
            ImGui::End();
        }
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    // Rendering
    ImGui::Render();
}

void CrossPlatformGUI::doInit()
{
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
  
    ImGuiIO& io = ImGui::GetIO(); 

    //TODO: Do scaling correctly
    //io.FontGlobalScale = 4;
    //ImGui::GetStyle().ScaleAllSizes(4);

    //TODO: remove in prod
#ifdef _DEBUG
    io.IniFilename = nullptr;
    io.WantSaveIniSettings = false;
#endif // DEBUG
    /*ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Arial.ttf", 15.0f);
    IM_ASSERT(font != NULL);*/
}
