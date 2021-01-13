#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "Constants.h"
#include "TimedMessageQueue.h"
#include "BluetoothWrapper.h"
#include "CrossPlatformGUI.h"

static void glfw_error_callback(int error, const char *description);
void EnterGUIMainLoop(BluetoothWrapper bt);