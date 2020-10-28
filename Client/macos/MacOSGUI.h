#pragma once

#include "imgui/imgui.h"
#include "Constants.h"
#include "IBluetoothConnector.h"
#include "BluetoothWrapper.h"
#include "CommandSerializer.h"
#include "Exceptions.h"
#include "TimedMessageQueue.h"
#include "SingleInstanceFuture.h"
#include "CascadiaCodeFont.h"
#include "MacOSBluetoothConnector.h"

#include <future>

constexpr auto GUI_MAX_MESSAGES = 10;
constexpr auto FPS = 60;
constexpr auto GUI_HEIGHT = 350;
constexpr auto GUI_WIDTH = 540;

//This class should be constructed after AFTER the Dear ImGUI context is initialized.
class MacOSGUI
{
public:
	MacOSGUI(BluetoothWrapper bt);

	//Run the GUI code once. This function should be called from a loop from one of the GUI impls (Windows, OSX, Linux...)
	//O: true if the user wants to close the window
	bool performGUIPass();
private:
	void _drawErrors();
	void _drawDeviceDiscovery();
	void _drawASMControls();

	BluetoothDevice _connectedDevice;
	BluetoothWrapper _bt;
	SingleInstanceFuture<std::vector<BluetoothDevice>> _connectedDevicesFuture;
	SingleInstanceFuture<int> _sendCommandFuture;
	SingleInstanceFuture<void> _connectFuture;
	TimedMessageQueue _mq;
};


