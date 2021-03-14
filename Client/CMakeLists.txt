project(SonyHeadphonesClient)
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

add_executable(client)

target_sources(
    client
    PRIVATE BluetoothWrapper.cpp
    ByteMagic.cpp
    CommandSerializer.cpp
    TimedMessageQueue.cpp
    Client.cpp
    CrossPlatformGUI.cpp)

target_include_directories (
    client PRIVATE ${CMAKE_SOURCE_DIR}
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

    target_include_directories (
        client PRIVATE ${CMAKE_SOURCE_DIR}/imgui
    )
endif()

if (WIN32)
    ADD_DEFINITIONS(-DUNICODE)

    target_sources(
        client
        PRIVATE windows/WindowsGUI.cpp
        imgui/backends/imgui_impl_dx11.cpp
        imgui/backends/imgui_impl_win32.cpp
        windows/WindowsBluetoothConnector.cpp
    )

    target_link_libraries(client d3d11)
endif (WIN32)

target_link_libraries(client Threads::Threads)
