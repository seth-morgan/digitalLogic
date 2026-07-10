cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)

add_executable(digitallogic_gate_tests
    GateTests.cpp
)

add_executable(digitallogic_circuit_tests
    CircuitTests.cpp
)

add_executable(digitallogic_serializer_tests
    CircuitSerializerTests.cpp
)

add_executable(digitallogic_simulation_tests
    SimulationEngineTests.cpp
)

target_link_libraries(digitallogic_gate_tests
    PRIVATE
        digitallogic_core
        Qt6::Test
)

target_link_libraries(digitallogic_circuit_tests
    PRIVATE
        digitallogic_core
        Qt6::Test
)

target_link_libraries(digitallogic_serializer_tests
    PRIVATE
        digitallogic_core
        Qt6::Test
)

target_link_libraries(digitallogic_simulation_tests
    PRIVATE
        digitallogic_core
        Qt6::Test
)

add_test(NAME digitallogic_gate_tests COMMAND digitallogic_gate_tests)
add_test(NAME digitallogic_circuit_tests COMMAND digitallogic_circuit_tests)
add_test(NAME digitallogic_serializer_tests COMMAND digitallogic_serializer_tests)
add_test(NAME digitallogic_simulation_tests COMMAND digitallogic_simulation_tests)
