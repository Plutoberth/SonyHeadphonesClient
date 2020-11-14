#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_metal.h"
#include <stdio.h>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

#include "Constants.h"
#include "TimedMessageQueue.h"
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