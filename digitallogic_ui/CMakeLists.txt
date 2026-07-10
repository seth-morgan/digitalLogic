cmake_minimum_required(VERSION 3.16)

add_library(digitallogic_ui STATIC
    include/digitallogic/ui/MainWindow.h
    include/digitallogic/ui/SandboxView.h
    include/digitallogic/ui/GatePaletteWidget.h
    include/digitallogic/ui/CircuitController.h
    include/digitallogic/ui/SimulationController.h
    include/digitallogic/ui/AppTheme.h
    include/digitallogic/ui/ChallengeController.h
    include/digitallogic/ui/WinCelebrationOverlay.h
    src/AppTheme.cpp
    include/digitallogic/ui/ChallengePanelWidget.h
    include/digitallogic/ui/ChallengeDialog.h
    include/digitallogic/ui/graphics/TargetGraphicsItem.h
    src/MainWindow.cpp
    src/SandboxView.cpp
    src/GatePaletteWidget.cpp
    src/CircuitController.cpp
    src/SimulationController.cpp
    src/ChallengeController.cpp
    src/WinCelebrationOverlay.cpp
    src/ChallengePanelWidget.cpp
    src/ChallengeDialog.cpp
    src/graphics/PinGraphicsItem.cpp
    src/graphics/SourceGraphicsItem.cpp
    src/graphics/GateGraphicsItem.cpp
    src/graphics/TargetGraphicsItem.cpp
    src/graphics/WireGraphicsItem.cpp
)

target_include_directories(digitallogic_ui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(digitallogic_ui
    PUBLIC
        digitallogic_core
        Qt6::Widgets
)

set_target_properties(digitallogic_ui PROPERTIES AUTOMOC ON)
