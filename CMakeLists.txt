cmake_minimum_required(VERSION 3.10)
project(GraphViz VERSION 1.3.0 LANGUAGES CXX)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# ── Qt6 ──
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
set(CMAKE_AUTOMOC ON)

# ── 源文件 ──
set(CORE_SOURCES
    src/Graph.cpp
    src/GraphParser.cpp
    src/GraphAlgorithm.cpp
)

set(GUI_SOURCES
    src/gui/MainWindow.cpp
    src/gui/GraphWidget.cpp
    src/gui/GraphTextEdit.cpp
    src/gui/ForceLayout.cpp
    src/gui/UpdateChecker.cpp
)

add_executable(GraphViz WIN32
    src/main.cpp
    ${CORE_SOURCES}
    ${GUI_SOURCES}
)

target_include_directories(GraphViz PRIVATE include src/gui)
target_link_libraries(GraphViz PRIVATE Qt6::Widgets Qt6::Network)
target_compile_definitions(GraphViz PRIVATE
    GRAPHVIZ_VERSION="${PROJECT_VERSION}")

if(WIN32)
    add_custom_command(TARGET GraphViz POST_BUILD
        COMMAND "${CMAKE_PREFIX_PATH}/bin/windeployqt.exe"
            --no-translations
            "$<TARGET_FILE:GraphViz>"
        COMMENT "Deploying Qt runtime DLLs..."
    )
endif()
