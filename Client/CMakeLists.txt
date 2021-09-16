project(SonyHeadphonesClient)
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

add_executable(SonyHeadphonesClient)

target_sources(
  SonyHeadphonesClient
  PRIVATE BluetoothWrapper.cpp ByteMagic.cpp CommandSerializer.cpp
          TimedMessageQueue.cpp)

target_include_directories(SonyHeadphonesClient PRIVATE ${CMAKE_SOURCE_DIR})

target_sources(
  SonyHeadphonesClient
  PRIVATE CascadiaCodeFont.cpp)

if(UNIX)
  target_sources(
    SonyHeadphonesClient
    PRIVATE linux/main.cpp)
  if(NOT APPLE)
    target_sources(
      SonyHeadphonesClient PRIVATE linux/DBusHelper.cpp
                                   linux/LinuxBluetoothConnector.cpp)
  else()
    target_sources(SonyHeadphonesClient
                   PRIVATE macos/MacOSBluetoothConnector.mm)
  endif()

  set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/linux)

  set(OpenGL_GL_PREFERENCE GLVND)

  if(NOT APPLE)
    find_package(DBus REQUIRED)
  endif()

  find_package(GLEW REQUIRED)

  find_package(glfw3 REQUIRED)

  find_package(OpenGL REQUIRED)

  if((NOT GLEW_FOUND)
     OR (NOT glfw3_FOUND)
     OR (NOT OPENGL_FOUND)
     OR (NOT APPLE AND NOT DBUS_FOUND))
    message(
      FATAL_ERROR
        "Didn't find one of the packages. For Debian based systems, use:"
        "sudo apt install libbluetooth-dev libglew-dev libglfw3-dev libdbus-1-dev"
    )
  endif()

  if(NOT APPLE)
    target_include_directories(SonyHeadphonesClient
                               PRIVATE ${DBUS_INCLUDE_DIRS})
  endif()

  target_include_directories(SonyHeadphonesClient
                             PRIVATE ${OPENGL_INCLUDE_DIRS})

  if(NOT APPLE)
    target_link_libraries(SonyHeadphonesClient PRIVATE ${DBUS_LIBRARIES}
                                                       bluetooth)
  endif()

  target_link_libraries(SonyHeadphonesClient PRIVATE GLEW::GLEW glfw OpenGL::GL)

  if(APPLE)
    find_library(BT_FRAMEWORK IOBluetooth REQUIRED)
    find_library(BT_UI_FRAMEWORK IOBluetoothUI REQUIRED)
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    target_link_libraries(
      SonyHeadphonesClient PRIVATE ${BT_UI_FRAMEWORK} ${BT_FRAMEWORK}
                                   ${COCOA_FRAMEWORK})
  endif()
endif()

if(WIN32)
  add_definitions(-DUNICODE)

  target_sources(
    SonyHeadphonesClient
    PRIVATE windows/main.cpp
            windows/WindowsBluetoothConnector.cpp)

  target_link_libraries(SonyHeadphonesClient PRIVATE d3d11)
endif()

target_link_libraries(SonyHeadphonesClient PRIVATE Threads::Threads)

add_subdirectory(qt)