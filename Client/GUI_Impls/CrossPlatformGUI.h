#pragma once

#include "imgui/imgui.h"
#include "Constants.h"
#include "BluetoothWrapper.h"
#include "CommandSerializer.h"

namespace CrossPlatformGUI
{
	//This function should be called AFTER the Dear ImGUI context is initialized.
	void doInit();

	//Run the GUI code once. This function should be called from a loop from one of the GUI impls (Windows, OSX, Linux...)
	void performGUIPass(BluetoothWrapper& bt);
};


