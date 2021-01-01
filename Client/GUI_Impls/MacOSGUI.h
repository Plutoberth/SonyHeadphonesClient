#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_metal.h"
#include <stdio.h>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <Metal/Metal.h>
#include <QuartzCore/QuartzCore.h>

#include "Constants.h"
#include "TimedMessageQueue.h"
#include "BluetoothWrapper.h"
#include "CrossPlatformGUI.h"

static void glfw_error_callback(int error, const char* description);
void EnterGUIMainLoop(BluetoothWrapper bt);