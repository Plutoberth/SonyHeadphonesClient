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

target_link_libraries(SonyHeadphonesClient PRIVATE Threads::Threads)

add_subdirectory(qt)