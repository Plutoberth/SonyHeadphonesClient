project(SonyHeadphonesClient)
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

add_executable(SonyHeadphonesClient)

target_sources(SonyHeadphonesClient
    PRIVATE BluetoothWrapper.cpp
    ByteMagic.cpp
    CommandSerializer.cpp
    TimedMessageQueue.cpp
    CrossPlatformGUI.cpp "Headphones.cpp")

target_include_directories (SonyHeadphonesClient
    PRIVATE ${CMAKE_SOURCE_DIR}
)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (WIN32 OR LINUX)
    set (DEAR_IMGUI TRUE)
endif()

if (DEAR_IMGUI)
    target_sources(SonyHeadphonesClient
        PRIVATE CrossPlatformGUI.cpp
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        CascadiaCodeFont.cpp
    )

    target_include_directories (SonyHeadphonesClient
        PRIVATE ${CMAKE_SOURCE_DIR}/imgui
    )
endif ()

if (LINUX)
	target_sources(SonyHeadphonesClient
		PRIVATE linux/DBusHelper.cpp
		linux/LinuxBluetoothConnector.cpp
		linux/LinuxGUI.cpp
        linux/main.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
    )

    set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/linux)

    set (OpenGL_GL_PREFERENCE GLVND)
	
    find_package(DBus REQUIRED)

    find_package(GLEW REQUIRED)

    find_package(glfw3 REQUIRED)

    find_package(OpenGL REQUIRED)

    if ((NOT DBUS_FOUND) OR (NOT GLEW_FOUND) OR (NOT glfw3_FOUND) OR (NOT OPENGL_FOUND))
        message ( FATAL_ERROR 
        "Didn't find one of the packages. For Debian based systems, use:"
        "sudo apt install libbluetooth-dev libglew-dev libglfw3-dev libdbus-1-dev" )
    endif ()

    target_include_directories(SonyHeadphonesClient
        PRIVATE ${DBUS_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
    )

    target_link_libraries(SonyHeadphonesClient
        PRIVATE ${DBUS_LIBRARIES}
        GLEW::GLEW 
        glfw
        bluetooth
        OpenGL::GL
    )

endif ()

if (WIN32)
    ADD_DEFINITIONS(-DUNICODE)

    target_sources(SonyHeadphonesClient
        PRIVATE windows/WindowsGUI.cpp
        windows/main.cpp
        windows/WindowsBluetoothConnector.cpp
        imgui/backends/imgui_impl_dx11.cpp
        imgui/backends/imgui_impl_win32.cpp
    )

    target_link_libraries(SonyHeadphonesClient
        PRIVATE d3d11
    )
endif ()

target_link_libraries(SonyHeadphonesClient
    PRIVATE Threads::Threads
)
