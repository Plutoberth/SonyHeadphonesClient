#pragma once

#include "imgui/imgui.h"
#include "Constants.h"
#include "IBluetoothConnector.h"
#include "BluetoothWrapper.h"
#include "CommandSerializer.h"
#include "RecoverableException.h"
#include "TimedMessageQueue.h"

#include <future>

constexpr auto GUI_MAX_MESSAGES = 10;

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
	bool _isConnectedDevicesFutureReady();

	BluetoothDevice _connectedDevice;
	BluetoothWrapper _bt;
	std::future<std::vector<BluetoothDevice>> _connectedDevicesFuture;
	TimedMessageQueue _mq;
};


