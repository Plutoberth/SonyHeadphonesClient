#include "CrossPlatformGUI.h"

bool CrossPlatformGUI::performGUIPass(BluetoothWrapper& bt)
{
    ImGui::NewFrame();
   
    static char MAC[MAC_ADDR_STR_SIZE + 1] = "38:18:4c:bf:44:7f";
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static bool isConnected = false;

    static std::vector<BluetoothDevice> connectedDevices = bt.getConnectedDevices();
    
    //Avoid issues with changing indexes
    static BluetoothDevice connectedDevice;
    bool open = true;

    ImGui::SetNextWindowPos({ 0,0 });
    {
        //ImGui::ShowDemoWindow();
        //TODO: Figure out how to get rid of the Windows window, make everything transparent, and just use ImGui for everything.
        ImGui::Begin("Sony Headphones", &open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);

        static int selectedDevice = -1;
        if (ImGui::CollapsingHeader("Device Discovery   ", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (isConnected)
            {
                ImGui::Text("Connected to %s (%s)", connectedDevice.name.c_str(), connectedDevice.mac.c_str());
                if (ImGui::Button("Disconnect"))
                {
                    isConnected = false;
                    bt.disconnect();
                }
            }
            else
            {
                ImGui::Text("Select from one of the available devices: ");

                int temp = 0;
                for (auto device : connectedDevices)
                {
                    auto name = device.name + " (" + device.mac + ")";
                    ImGui::RadioButton(name.c_str(), &selectedDevice, temp++);
                }

                ImGui::Spacing();

                if (ImGui::Button("Connect"))
                {
                    if (selectedDevice != -1)
                    {
                        connectedDevice = connectedDevices[selectedDevice];
                        //TODO: This isn't proper at all. Call may block
                        bt.connect(connectedDevice.mac);
                        isConnected = true;
                    }
                }

                ImGui::SameLine();

                if (ImGui::Button("Refresh devices"))
                {
                    //refresh connectedDevices
                }
            }
        }
        

        ImGui::Spacing();

        if (isConnected)
        {
            static bool focusOnVoice = false;
            static bool sentFocusOnVoice = focusOnVoice;
            static int asmLevel = 0;
            static int sentAsmLevel = asmLevel;

            if (ImGui::CollapsingHeader("Ambient Sound Mode   "))
            {
                //ImGui::Begin("Ambient Sound Mode");                          // Create a window called "Hello, world!" and append into it.
                ImGui::Text("Control ambient sound for your %ss", "WH-1000-XM3");

                ImGui::SliderInt("Ambient Sound Level", &asmLevel, 0, 19);

                bool sliderActive = ImGui::IsItemActive();

                ImGui::Checkbox("Focus on Voice", &focusOnVoice);

                if (sentAsmLevel != asmLevel || sentFocusOnVoice != focusOnVoice)
                {
                    auto ncAsmEffect = sliderActive ? NC_ASM_EFFECT::ADJUSTMENT_IN_PROGRESS : NC_ASM_EFFECT::ADJUSTMENT_COMPLETION;
                    auto asmId = focusOnVoice ? ASM_ID::VOICE : ASM_ID::NORMAL;

                    //TODO: Check if sliderActive actually works properly

                    bt.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
                        ncAsmEffect,
                        NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                        0,
                        ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
                        asmId,
                        asmLevel
                    ));
                   
                    sentAsmLevel = asmLevel;
                    sentFocusOnVoice = focusOnVoice;
                }
            }
        }
        ImGui::End();
    }

    // Rendering
    ImGui::Render();

    return open;
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
