project(SonyHeadphonesClient)
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

add_executable(client)

target_sources(client
    PRIVATE BluetoothWrapper.cpp
    ByteMagic.cpp
    CommandSerializer.cpp
    TimedMessageQueue.cpp
    CrossPlatformGUI.cpp)

target_include_directories (client
    PRIVATE ${CMAKE_SOURCE_DIR}
)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (WIN32 OR LINUX)
    set (DEAR_IMGUI TRUE)
endif()

if (DEAR_IMGUI)
    target_sources(
        client
        PRIVATE CrossPlatformGUI.cpp
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        CascadiaCodeFont.cpp
    )

    target_include_directories (client
        PRIVATE ${CMAKE_SOURCE_DIR}/imgui
    )
endif ()

if (LINUX)
	target_sources(
		client
		PRIVATE linux/DBusHelper.cpp
		linux/LinuxBluetoothConnector.cpp
		linux/LinuxGUI.cpp
        linux/main.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
    )

    set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/linux)
	
    find_package(DBus REQUIRED)

    find_package(GLEW REQUIRED)

    find_package(glfw3 REQUIRED)

    find_package(OpenGL REQUIRED)

    # Add if for glew, dbus, glfw not found

    target_include_directories(client
        PRIVATE ${DBUS_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
    )

    target_link_libraries(client
        PRIVATE ${DBUS_LIBRARIES}
        GLEW::GLEW 
        glfw
        bluetooth
        ${OPENGL_LIBRARIES}
    )

endif ()

if (WIN32)
    ADD_DEFINITIONS(-DUNICODE)

    target_sources(client
        PRIVATE windows/WindowsGUI.cpp
        windows/main.cpp
        windows/WindowsBluetoothConnector.cpp
        imgui/backends/imgui_impl_dx11.cpp
        imgui/backends/imgui_impl_win32.cpp
    )

    target_link_libraries(client 
        PRIVATE d3d11
    )
endif ()

target_link_libraries(client 
    PRIVATE Threads::Threads
)
