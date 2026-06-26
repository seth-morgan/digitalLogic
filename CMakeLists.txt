cmake_minimum_required(VERSION 3.16)

project(DigitalLogicSim VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Test)

enable_testing()

add_subdirectory(digitallogic_core)
add_subdirectory(digitallogic_ui)
add_subdirectory(app)
add_subdirectory(tests)
