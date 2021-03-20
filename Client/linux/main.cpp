#include <iostream>

#include "BluetoothWrapper.h"
#include "LinuxGUI.h"

#ifndef __APPLE__
#include "LinuxBluetoothConnector.h"
#define BluetoothConnector LinuxBluetoothConnector
#else
#include "../macos/MacOSBluetoothConnector.h"
#define BluetoothConnector MacOSBluetoothConnector
#endif

int main(int argc, const char *argv[])
{
    try
    {
        // Get bluetooth connector
        std::unique_ptr<IBluetoothConnector> connector = std::make_unique<BluetoothConnector>();
        // Wrap the connector using the bluetoothwrapper
        BluetoothWrapper wrap(std::move(connector));
        // Start the gui
        EnterGUIMainLoop(std::move(wrap));
    }
    catch (const std::exception &e)
    {
		  std::cout << e.what();
	  }
    return 1;
}
