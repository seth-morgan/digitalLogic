cmake_minimum_required(VERSION 3.16)

add_executable(DigitalLogicSim
    main.cpp
)

target_link_libraries(DigitalLogicSim
    PRIVATE
        digitallogic_ui
        Qt6::Widgets
)

set_target_properties(DigitalLogicSim PROPERTIES
    WIN32_EXECUTABLE TRUE
)
