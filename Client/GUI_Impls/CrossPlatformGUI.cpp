#include "CrossPlatformGUI.h"

bool CrossPlatformGUI::performGUIPass()
{
	ImGui::NewFrame();

	static bool isConnected = false;

	//Avoid issues with changing indexes
	static BluetoothDevice connectedDevice;
	bool open = true;

	//To report errors that don't cause any invalid state, and may be retried.

	ImGui::SetNextWindowPos({ 0,0 });

	{

	}

	{
		//ImGui::ShowDemoWindow();
		//TODO: Figure out how to get rid of the Windows window, make everything transparent, and just use ImGui for everything.
		ImGui::Begin("Sony Headphones", &open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);

		//There's a slight race condition here but I don't care
		if (this->_mq.begin() != this->_mq.end())
		{
			ImGui::Text("Errors:");
			for (auto&& message : this->_mq)
			{
				ImGui::Text(message.message.c_str());
			}
			//TODO: add a better ending separator
			ImGui::Text("----------------------------");
		}

		if (ImGui::CollapsingHeader("Device Discovery   ", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static std::vector<BluetoothDevice> connectedDevices;
			static int selectedDevice = -1;
	
			if (isConnected)
			{
				ImGui::Text("Connected to %s (%s)", connectedDevice.name.c_str(), connectedDevice.mac.c_str());
				if (ImGui::Button("Disconnect"))
				{
					isConnected = false;
					this->_bt.disconnect();
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
						this->_bt.connect(connectedDevice.mac);
						isConnected = true;
					}
				}

				ImGui::SameLine();

				if (this->_optionalConnectedDevicesFuture.has_value())
				{
					if (this->_isConnectedDevicesFutureReady())
					{ 
						try
						{
							connectedDevices = this->_optionalConnectedDevicesFuture.value().get();
						}
						catch (const RecoverableException& exc)
						{
							this->_mq.addMessage(exc.what());
						}
						
						this->_optionalConnectedDevicesFuture.reset();
					}
					else
					{
						auto dots = { ".", "..", "..." };
						ImGui::Text("Discovering Devices %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
					}
				}
				else
				{
					if (ImGui::Button("Refresh devices"))
					{
						this->_setConnectedDevicesFuture();
					}
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

			if (ImGui::CollapsingHeader("Ambient Sound Mode   ", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("Control ambient sound for your %ss", connectedDevice.name.c_str());

				ImGui::SliderInt("Ambient Sound Level", &asmLevel, 0, 19);

				bool sliderActive = ImGui::IsItemActive();

				ImGui::Checkbox("Focus on Voice", &focusOnVoice);

				if (sentAsmLevel != asmLevel || sentFocusOnVoice != focusOnVoice)
				{
					auto ncAsmEffect = sliderActive ? NC_ASM_EFFECT::ADJUSTMENT_IN_PROGRESS : NC_ASM_EFFECT::ADJUSTMENT_COMPLETION;
					auto asmId = focusOnVoice ? ASM_ID::VOICE : ASM_ID::NORMAL;

					this->_bt.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
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

void CrossPlatformGUI::_setConnectedDevicesFuture()
{
	auto boundFunction = std::bind(&BluetoothWrapper::getConnectedDevices, std::ref(this->_bt));
	this->_optionalConnectedDevicesFuture.emplace(std::async(std::launch::async, boundFunction));
}

bool CrossPlatformGUI::_isConnectedDevicesFutureReady()
{
	if (!this->_optionalConnectedDevicesFuture.has_value())
	{
		return false;
	}
	return this->_optionalConnectedDevicesFuture.value().wait_for(std::chrono::seconds(0))  == std::future_status::ready;
}

CrossPlatformGUI::CrossPlatformGUI(BluetoothWrapper bt) : _bt(std::move(bt))
{
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	this->_mq = TimedMessageQueue(GUI_MAX_MESSAGES);
	this->_setConnectedDevicesFuture();

	//TODO: Do scaling correctly
	//io.FontGlobalScale = 4;
	//ImGui::GetStyle().ScaleAllSizes(4);

#ifdef _DEBUG
	io.IniFilename = nullptr;
	io.WantSaveIniSettings = false;
#endif // DEBUG

	//For saner development
#ifdef _DEBUG
	ImFont* font = io.Fonts->AddFontFromFileTTF("./Fonts/CascadiaCode.ttf", 15.0f);
	IM_ASSERT(font != NULL);
#endif
}
