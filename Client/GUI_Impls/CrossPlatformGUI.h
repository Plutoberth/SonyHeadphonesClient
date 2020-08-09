#pragma once

#include "imgui/imgui.h"
#include "Constants.h"
#include "IBluetoothConnector.h"
#include "BluetoothWrapper.h"
#include "CommandSerializer.h"

#include <optional>
#include <future>

//This class should be constructed after AFTER the Dear ImGUI context is initialized.
class CrossPlatformGUI
{
public:
	CrossPlatformGUI(BluetoothWrapper bt);

	//Run the GUI code once. This function should be called from a loop from one of the GUI impls (Windows, OSX, Linux...)
	//O: true if the user wants to close the window
	bool performGUIPass();
private:
	void _setConnectedDevicesFuture();

	BluetoothWrapper _bt;
	std::optional<std::future<std::vector<BluetoothDevice>>> _optionalConnectedDevicesFuture;
};


