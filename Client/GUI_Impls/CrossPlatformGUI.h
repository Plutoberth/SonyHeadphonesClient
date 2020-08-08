#pragma once

#include "imgui/imgui.h"
#include "Constants.h"
#include "IBluetoothConnector.h"
#include "BluetoothWrapper.h"
#include "CommandSerializer.h"

namespace CrossPlatformGUI
{
	//This function should be called AFTER the Dear ImGUI context is initialized.
	void doInit();

	//Run the GUI code once. This function should be called from a loop from one of the GUI impls (Windows, OSX, Linux...)
	//I: BluetoothWrapper
	//O: true if the user wants to close the window
	bool performGUIPass(BluetoothWrapper& bt);
};


