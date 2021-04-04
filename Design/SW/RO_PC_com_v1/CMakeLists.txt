cmake_minimum_required(VERSION 3.17)
project(RO_PC_com_v1)

set(CMAKE_CXX_STANDARD 20)

add_executable(RO_PC_com_v1 main.cpp SerialPort.cpp SerialPort.h dataParser.cpp dataParser.h stateMachine.cpp stateMachine.h socketHandler.cpp socketHandler.h)