#include <stdio.h>
#include "LinuxGUI.h"
#include "LinuxBluetoothConnector.h"
#include "BluetoothWrapper.h"

int main(int argc, const char * argv[])
{
    try {
        // Get bluetooth connector
        std::unique_ptr<IBluetoothConnector> connector = std::make_unique<LinuxBluetoothConnector>();
        // Wrap the connector using the bluetoothwrapper
        BluetoothWrapper wrap(std::move(connector));
        // Start the gui
        EnterGUIMainLoop(std::move(wrap));
    } catch (const std::exception& e){
		printf("%s",e.what());
	}
    return 1;
}
