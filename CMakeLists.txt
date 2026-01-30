cmake_minimum_required(VERSION 3.10)

project(SerialTemperature)

set(CMAKE_CXX_STANDARD 17)

add_executable(temp_reader
    src/temp_reader.cpp
)

add_executable(temp_simulator
    src/temp_simulator.cpp
)

if(UNIX)
    target_link_libraries(temp_reader pthread)
endif()
