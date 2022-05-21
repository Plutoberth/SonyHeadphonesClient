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
			this->_drawSurroundControls();
			this->_setHeadphoneSettings();
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
		ImGui::Spacing();

		for (auto&& message : this->_mq)
		{
			ImGui::Text(message.message.c_str());
		}
		
		ImGui::Spacing();
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
			for (const auto& device : connectedDevices)
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
						if (exc.shouldDisconnect)
						{
							this->_bt.disconnect();
						}
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
						if (exc.shouldDisconnect)
						{
							this->_bt.disconnect();
						}
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
					selectedDevice = -1;
					this->_connectedDevicesFuture.setFromAsync([this]() { return this->_bt.getConnectedDevices(); });
				}
			}
		}
	}
}

void CrossPlatformGUI::_drawASMControls()
{
	static bool ambientSoundControl = true;
	static bool focusOnVoice = false;
	static int asmLevel = 0;

	if (ImGui::CollapsingHeader("Ambient Sound Mode   ", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Ambient Sound Control", &ambientSoundControl);

		if (this->_headphones.isSetAsmLevelAvailable())
		{
			ImGui::Text("Control ambient sound for your %ss", this->_connectedDevice.name.c_str());

			ImGui::SliderInt("Ambient Sound Level", &asmLevel, 0, 19);

			if (this->_headphones.isFocusOnVoiceAvailable())
			{
				ImGui::Checkbox("Focus on Voice", &focusOnVoice);
			}
			else
			{
				ImGui::Text("Focus on Voice isn't enabled on this level.");
			}
		}

		this->_headphones.setAmbientSoundControl(ambientSoundControl);
		this->_headphones.setAsmLevel(asmLevel);
		this->_headphones.setFocusOnVoice(focusOnVoice);
	}
}

void CrossPlatformGUI::_drawSurroundControls()
{
	static int soundPosition = 0;
	static int vptType = 0;

	if (ImGui::CollapsingHeader("Virtual Sound", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Only one of the options may be used at a time");

		if (ImGui::Combo("Sound Position", &soundPosition, "Off\0Front Left\0Front Right\0"
			"Front\0Rear Left\0Rear Right\0\0"))
		{
			vptType = 0;
		}

		if (ImGui::Combo("Surround (VPT)", &vptType, "Off\0Outdoor Festival\0Arena\0"
			"Concert Hall\0Club\0\0"))
		{
			soundPosition = 0;
		}

		this->_headphones.setSurroundPosition(SOUND_POSITION_PRESET_ARRAY[soundPosition]);
		this->_headphones.setVptType(vptType);
	}
}

void CrossPlatformGUI::_setHeadphoneSettings() {
	//Don't show if the command only takes a few frames to send
	static int commandLinger = 0;

	if (this->_sendCommandFuture.ready())
	{
		commandLinger = 0;
		try
		{
			this->_sendCommandFuture.get();
		}
		catch (const RecoverableException& exc)
		{
			std::string excString;
			//We kinda have to do it here and not in the wrapper, due to async causing timing issues. To fix it, the messagequeue can be made
			//static, but I'm not sure if I wanna do that.
			if (exc.shouldDisconnect)
			{
				this->_bt.disconnect();
				excString = "Disconnected due to: ";
			}
			this->_mq.addMessage(excString + exc.what());
		}
	}
	//This means that we're waiting
	else if (this->_sendCommandFuture.valid())
	{
		if (commandLinger++ > (FPS / 10))
		{
			ImGui::Text("Sending command %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
		}
	}
	//We're not waiting, and there's no command in the air, so we can evaluate sending a new command
	else if (this->_headphones.isChanged())
	{
		this->_sendCommandFuture.setFromAsync([=, this]() {
			return this->_headphones.setChanges();
		});
	}
}

CrossPlatformGUI::CrossPlatformGUI(BluetoothWrapper bt) : _bt(std::move(bt)), _headphones(_bt)
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
	ImFont* font = io.Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(fileData), sizeof(CascadiaCodeTTF), FONT_SIZE);
	IM_ASSERT(font != NULL);
}
