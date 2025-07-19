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
#include "Headphones.h"
#include "Listener.h"

#include <iostream>
#include <future>
#include <string>

constexpr auto GUI_MAX_MESSAGES = 5;
constexpr auto GUI_HEIGHT = 380;
constexpr auto GUI_WIDTH = 540;
constexpr auto FPS = 60;
constexpr auto MS_PER_FRAME = 1000 / FPS;
constexpr auto FONT_SIZE = 15.0f;
const auto WINDOW_COLOR = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

//This class should be constructed after AFTER the Dear ImGUI context is initialized.
class CrossPlatformGUI
{
public:
	CrossPlatformGUI(BluetoothWrapper bt);

	//Run the GUI code once. This function should be called from a loop from one of the GUI impls (Windows, OSX, Linux...)
	//O: true if the user wants to close the window
	bool performGUIPass();
private:
	void _drawErrors();
	void _drawDeviceDiscovery();
	void _drawASMControls();
	void _drawSurroundControls();
	void _setHeadphoneSettings();
	void _drawOptimizerButton();
	void _drawSpeakToChat();
	void _drawMultiPointConn();

	BluetoothDevice _connectedDevice;
	BluetoothWrapper _bt;
	SingleInstanceFuture<std::vector<BluetoothDevice>> _connectedDevicesFuture;
	SingleInstanceFuture<void> _sendCommandFuture;
	SingleInstanceFuture<void> _connectFuture;
	TimedMessageQueue _mq;
	Headphones _headphones;
	// Listener _listener;
	std::unique_ptr<Listener> _listener;
};


