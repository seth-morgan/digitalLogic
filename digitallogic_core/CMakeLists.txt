cmake_minimum_required(VERSION 3.16)

add_library(digitallogic_core STATIC
    src/model/SignalValue.cpp
    src/model/ComponentId.cpp
    src/model/PinId.cpp
    src/model/SourceNode.cpp
    src/model/Gate.cpp
    src/gates/AndGate.cpp
    src/gates/OrGate.cpp
    src/gates/NotGate.cpp
    src/model/Wire.cpp
    src/model/Circuit.cpp
    src/model/SimulationEngine.cpp
)

target_include_directories(digitallogic_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(digitallogic_core
    PUBLIC
        Qt6::Core
)
