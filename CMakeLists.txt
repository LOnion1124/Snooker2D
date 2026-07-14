cmake_minimum_required(VERSION 3.20)
project(Snooker2D VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Common 层（纯 C++，不依赖 Qt）
add_library(snooker_common STATIC
    src/common/MathUtils.cpp
)

target_include_directories(snooker_common PUBLIC
    ${CMAKE_SOURCE_DIR}/src/common
)

# Model 层（依赖 Common + Qt Core）
add_library(snooker_model STATIC
    src/model/Ball.cpp
    src/model/Table.cpp
    src/model/Physics.cpp
    src/model/GameState.cpp
    src/model/Player.cpp
    src/model/Rules.cpp
)

target_include_directories(snooker_model PUBLIC
    ${CMAKE_SOURCE_DIR}/src/model
    ${CMAKE_SOURCE_DIR}/src/common
)

target_link_libraries(snooker_model PUBLIC
    snooker_common
    Qt6::Core
)

# ViewModel 层（依赖 Model + Qt Core）
add_library(snooker_viewmodel STATIC
    src/viewmodel/GameSessionViewModel.cpp
)

target_include_directories(snooker_viewmodel PUBLIC
    ${CMAKE_SOURCE_DIR}/src/viewmodel
    ${CMAKE_SOURCE_DIR}/src/model
    ${CMAKE_SOURCE_DIR}/src/common
)

target_link_libraries(snooker_viewmodel PUBLIC
    snooker_model
    snooker_common
    Qt6::Core
)

# View 层（依赖 Common + Qt Widgets，不依赖 ViewModel/Model）
add_library(snooker_view STATIC
    src/view/GameView.cpp
    src/view/CueControl.cpp
    src/view/ScoreBoard.cpp
    src/view/GameInfoPanel.cpp
    src/view/MainWindow.cpp
)

target_include_directories(snooker_view PUBLIC
    ${CMAKE_SOURCE_DIR}/src/view
    ${CMAKE_SOURCE_DIR}/src/common
)

target_link_libraries(snooker_view PUBLIC
    snooker_common
    Qt6::Widgets
)

# 主可执行文件（App 层负责装配 MVVM）
add_executable(Snooker2D
    src/main.cpp
    src/app/App.cpp
)

target_link_libraries(Snooker2D PRIVATE
    snooker_view
    snooker_viewmodel
    snooker_model
    snooker_common
    Qt6::Widgets
)

# 单元测试
add_executable(test_mathutils
    tests/test_mathutils.cpp
)

target_link_libraries(test_mathutils PRIVATE
    snooker_common
)

# 架构护栏检查
add_custom_target(check_architecture
    COMMAND powershell -ExecutionPolicy Bypass
            -File ${CMAKE_SOURCE_DIR}/tests/architecture/check_mvvm_boundaries.ps1
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running MVVM architecture boundary checks..."
)
