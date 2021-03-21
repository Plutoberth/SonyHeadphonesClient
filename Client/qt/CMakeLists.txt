set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5Widgets REQUIRED)
add_definitions(
  -DQT_NO_CAST_FROM_ASCII
  -DQT_NO_CAST_FROM_BYTEARRAY
  -DQT_NO_CAST_TO_ASCII
  -DQT_NO_SIGNALS_SLOTS_KEYWORDS
  -DQT_NO_URL_CAST_FROM_STRING
  -DQT_STRICT_ITERATORS
  -DQT_USE_FAST_OPERATOR_PLUS
  -DQT_USE_QSTRINGBUILDER)
add_executable(
  SonyHeadphonesClient-qt MACOSX_BUNDLE
  ../BluetoothWrapper.cpp ../ByteMagic.cpp ../CommandSerializer.cpp
  ../TimedMessageQueue.cpp Dialog.cpp main.cpp)
if(UNIX AND NOT APPLE)
  target_sources(
    SonyHeadphonesClient-qt PRIVATE ../linux/DBusHelper.cpp
                                    ../linux/LinuxBluetoothConnector.cpp)
  target_link_libraries(SonyHeadphonesClient-qt PRIVATE bluetooth
                                                        ${DBUS_LIBRARIES})
  target_include_directories(SonyHeadphonesClient-qt
                             PRIVATE ${DBUS_INCLUDE_DIRS})
elseif(WIN32)
  # See WindowsBluetoothConnector.h for linking pragmas
  add_compile_definitions(UNICODE)
  target_sources(SonyHeadphonesClient-qt
                 PRIVATE ../windows/WindowsBluetoothConnector.cpp)
elseif(APPLE)
  target_sources(SonyHeadphonesClient-qt
                 PRIVATE ../macos/MacOSBluetoothConnector.mm)
  target_link_libraries(
    SonyHeadphonesClient-qt PRIVATE ${BT_UI_FRAMEWORK} ${BT_FRAMEWORK}
                                    ${COCOA_FRAMEWORK})
endif()
target_include_directories(SonyHeadphonesClient-qt
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(SonyHeadphonesClient-qt PRIVATE Qt5::Widgets
                                                      Threads::Threads)
