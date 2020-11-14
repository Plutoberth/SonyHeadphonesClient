#include <stdio.h>

#include "MacOSGUI.h"
#include "MacOSBluetoothConnector.h"
#include "BluetoothWrapper.h"

int main(int, char**)
{
    // Get bluetooth connector
    std::unique_ptr<IBluetoothConnector> connector = std::make_unique<MacOSBluetoothConnector>();
    // Wrap the connector using the bluetoothwrapper
    BluetoothWrapper wrap(std::move(connector));
    // Start the gui
    EnterGUIMainLoop(std::move(wrap));
    return 0;
}