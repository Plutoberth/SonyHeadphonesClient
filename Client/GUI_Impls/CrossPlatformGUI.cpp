#include "CrossPlatformGUI.h"

bool CrossPlatformGUI::performGUIPass()
{
	ImGui::NewFrame();

	static bool isConnected = false;

	bool open = true;

	ImGui::SetNextWindowPos({ 0,0 });

	{
		//TODO: Figure out how to get rid of the Windows window, make everything transparent, and just use ImGui for everything.
		ImGui::Begin("Sony Headphones", &open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);

		//Legal disclaimer
		ImGui::Text("! This product is not affiliated with Sony. Use at your own risk. !");

		this->_drawErrors();
		this->_drawDeviceDiscovery();

		if (this->_bt.isConnected())
		{
			this->_drawASMControls();
		}

		ImGui::Spacing();
	}

	ImGui::End();
	ImGui::Render();

	return open;
}

void CrossPlatformGUI::_setConnectedDevicesFuture()
{
	if (this->_connectedDevicesFuture.valid())
	{
		throw std::runtime_error("The asynchronous action was cancelled before it finished executing");
	}
	
	auto boundFunction = [this]() { return this->_bt.getConnectedDevices(); };
	this->_connectedDevicesFuture = std::async(std::launch::async, boundFunction);
}

bool CrossPlatformGUI::_isConnectedDevicesFutureReady()
{
	if (!this->_connectedDevicesFuture.valid())
	{
		return false;
	}
	return this->_connectedDevicesFuture.wait_for(std::chrono::seconds(0))  == std::future_status::ready;
}

void CrossPlatformGUI::_drawErrors()
{
	//There's a slight race condition here but I don't care, it'd only be for one frame.
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
}

void CrossPlatformGUI::_drawDeviceDiscovery()
{
	if (ImGui::CollapsingHeader("Device Discovery   ", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static std::vector<BluetoothDevice> connectedDevices;
		static int selectedDevice = -1;

		if (this->_bt.isConnected())
		{
			ImGui::Text("Connected to %s (%s)", this->_connectedDevice.name.c_str(), this->_connectedDevice.mac.c_str());
			if (ImGui::Button("Disconnect"))
			{
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
					this->_connectedDevice = connectedDevices[selectedDevice];
					//TODO: This isn't proper at all. Call may block
					this->_bt.connect(this->_connectedDevice.mac);
				}
			}

			ImGui::SameLine();

			if (this->_connectedDevicesFuture.valid())
			{
				if (this->_isConnectedDevicesFutureReady())
				{
					try
					{
						connectedDevices = this->_connectedDevicesFuture.get();
					}
					catch (const RecoverableException& exc)
					{
						this->_mq.addMessage(exc.what());
					}
				}
				else
				{
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
}

void CrossPlatformGUI::_drawASMControls()
{
	static bool focusOnVoice = false;
	static bool sentFocusOnVoice = focusOnVoice;
	static int asmLevel = 0;
	static int sentAsmLevel = asmLevel;

	if (ImGui::CollapsingHeader("Ambient Sound Mode   ", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Control ambient sound for your %ss", this->_connectedDevice.name.c_str());

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
