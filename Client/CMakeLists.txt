project(SonyHeadphonesClient)
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(SonyHeadphonesClient BluetoothWrapper.cpp ByteMagic.cpp
                                    CommandSerializer.cpp TimedMessageQueue.cpp
                                    Headphones.cpp)

if(NOT APPLE)
  add_definitions(
    -DQT_NO_CAST_FROM_ASCII 
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_STRICT_ITERATORS
    -DQT_USE_FAST_OPERATOR_PLUS
    -DQT_USE_QSTRINGBUILDER)

  find_package(Qt6 REQUIRED COMPONENTS Widgets)
  qt_standard_project_setup()
  target_link_libraries(SonyHeadphonesClient PRIVATE Qt6::Widgets)

  target_sources(SonyHeadphonesClient PRIVATE qt/Dialog.cpp qt/main.cpp qt/dialog.ui)

  qt_add_resources(SonyHeadphonesClient "stylesheet"
    PREFIX "/"
    FILES
     qt/resources/Kuroi.qss
  )
endif()

if(NOT APPLE)
  if(UNIX)
    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/linux)

    find_package(DBus REQUIRED)

    if(NOT DBUS_FOUND)
      message(FATAL_ERROR "Didn't find dbus. For Debian based systems, use:"
                          "sudo apt install libdbus-1-dev")
    endif()

    target_sources(
      SonyHeadphonesClient PRIVATE linux/DBusHelper.cpp
                                   linux/LinuxBluetoothConnector.cpp)

    target_link_libraries(SonyHeadphonesClient PRIVATE bluetooth
                                                       ${DBUS_LIBRARIES})
    target_include_directories(SonyHeadphonesClient
                               PRIVATE ${DBUS_INCLUDE_DIRS})
  elseif(WIN32)
    # See WindowsBluetoothConnector.h for linking pragmas
    add_compile_definitions(UNICODE)
    target_sources(SonyHeadphonesClient
                   PRIVATE windows/WindowsBluetoothConnector.cpp)
  endif()
elseif(APPLE)
  target_sources(SonyHeadphonesClient PRIVATE macos/MacOSBluetoothConnector.mm)

  find_library(BT_FRAMEWORK IOBluetooth REQUIRED)
  find_library(BT_UI_FRAMEWORK IOBluetoothUI REQUIRED)
  find_library(COCOA_FRAMEWORK Cocoa REQUIRED)

  target_link_libraries(
    SonyHeadphonesClient PRIVATE ${BT_UI_FRAMEWORK} ${BT_FRAMEWORK}
                                 ${COCOA_FRAMEWORK})
endif()

target_include_directories(SonyHeadphonesClient
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SonyHeadphonesClient PRIVATE Threads::Threads)
