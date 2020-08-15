#include "CrossPlatformGUI.h"

bool CrossPlatformGUI::performGUIPass()
{
	ImGui::NewFrame();

	static bool isConnected = false;

	bool open = true;

	ImGui::SetNextWindowPos({ 0,0 });

	{
		//TODO: Figure out how to get rid of the Windows window, make everything transparent, and just use ImGui for everything.
		//TODO: ImGuiWindowFlags_AlwaysAutoResize causes some flickering. Figure out how to stop it
		ImGui::Begin("Sony Headphones", &open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);

		//Legal disclaimer
		ImGui::Text("! This product is not affiliated with Sony. Use at your own risk. !");
		ImGui::Text("Source: https://github.com/Plutoberth/SonyHeadphonesClient");
		ImGui::Spacing();

		this->_drawErrors();
		this->_drawDeviceDiscovery();

		if (this->_bt.isConnected())
		{
			ImGui::Spacing();
			this->_drawASMControls();
		}
	}

	ImGui::End();
	ImGui::Render();

	return open;
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
			ImGui::Text("Connected to %s", this->_connectedDevice.name.c_str());
			if (ImGui::Button("Disconnect"))
			{
				selectedDevice = -1;
				this->_bt.disconnect();
			}
		}
		else
		{
			ImGui::Text("Select from one of the available devices: ");

			int temp = 0;
			for (auto device : connectedDevices)
			{
				ImGui::RadioButton(device.name.c_str(), &selectedDevice, temp++);
			}

			ImGui::Spacing();

			if (this->_connectFuture.valid())
			{
				if (this->_connectFuture.ready())
				{
					try
					{
						this->_connectFuture.get();
					}
					catch (const RecoverableException& exc)
					{
						this->_mq.addMessage(exc.what());
					}
				}
				else
				{
					ImGui::Text("Connecting %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
				}
			}
			else
			{
				if (ImGui::Button("Connect"))
				{
					if (selectedDevice != -1)
					{
						this->_connectedDevice = connectedDevices[selectedDevice];
						this->_connectFuture.setFromAsync([this]() { this->_bt.connect(this->_connectedDevice.mac); });
					}
				}
			}

			ImGui::SameLine();

			if (this->_connectedDevicesFuture.valid())
			{
				if (this->_connectedDevicesFuture.ready())
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
					this->_connectedDevicesFuture.setFromAsync([this]() { return this->_bt.getConnectedDevices(); });
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
			if (!this->_sendCommandFuture.valid())
			{
				auto ncAsmEffect = sliderActive ? NC_ASM_EFFECT::ADJUSTMENT_IN_PROGRESS : NC_ASM_EFFECT::ADJUSTMENT_COMPLETION;
				auto asmId = focusOnVoice ? ASM_ID::VOICE : ASM_ID::NORMAL;

				this->_sendCommandFuture.setFromAsync([=]() {
					return this->_bt.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
						ncAsmEffect,
						NC_ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
						0,
						ASM_SETTING_TYPE::LEVEL_ADJUSTMENT,
						asmId,
						asmLevel
					));
				});
				sentAsmLevel = asmLevel;
				sentFocusOnVoice = focusOnVoice;
			}
			else if (this->_sendCommandFuture.ready())
			{
				try
				{
					this->_sendCommandFuture.get();
				}
				catch (const RecoverableException& exc)
				{
					this->_mq.addMessage(exc.what());
				}
			}
		}
	}
}

CrossPlatformGUI::CrossPlatformGUI(BluetoothWrapper bt) : _bt(std::move(bt))
{
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	this->_mq = TimedMessageQueue(GUI_MAX_MESSAGES);
	this->_connectedDevicesFuture.setFromAsync([this]() { return this->_bt.getConnectedDevices(); });

	io.IniFilename = nullptr;
	io.WantSaveIniSettings = false;

	//AddFontFromMemory will own the pointer, so there's no leak
	char* fileData = new char[sizeof(CascadiaCodeTTF)];
	memcpy(fileData, CascadiaCodeTTF, sizeof(CascadiaCodeTTF));
	ImFont* font = io.Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(fileData), sizeof(CascadiaCodeTTF), 15.0f);
	IM_ASSERT(font != NULL);
}
