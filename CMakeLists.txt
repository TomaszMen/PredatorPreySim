cmake_minimum_required(VERSION 3.16)
project(PredatorPreySimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

add_executable(PredatorPreySimulation
    main.cpp
    mainwindow.h mainwindow.cpp
    simulationwidgets.h simulationwidgets.cpp
    sidepanel.h sidepanel.cpp
    organism.h organism.cpp
    prey.h prey.cpp
    predator.h predator.cpp
    environment.h environment.cpp
    herd.h herd.cpp
)

target_link_libraries(PredatorPreySimulation
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
)

# Opcjonalnie: ustaw właściwości specyficzne dla Qt6
set_target_properties(PredatorPreySimulation PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
