cmake_minimum_required(VERSION 3.20)
project(Snooker2D VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# Qt 6 依赖（通过 vcpkg 或系统安装）
# ============================================================================
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# ============================================================================
# Common 层（静态库，纯 C++，不依赖 Qt）
# ============================================================================
add_library(snooker_common STATIC
    src/common/MathUtils.cpp
)

target_include_directories(snooker_common PUBLIC
    ${CMAKE_SOURCE_DIR}/src/common
)

# ============================================================================
# Model 层（静态库，依赖 Common + Qt Core）
# ============================================================================
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

# ============================================================================
# ViewModel 层（静态库，依赖 Model + Qt Core）
# ============================================================================
add_library(snooker_viewmodel STATIC
    src/viewmodel/GameViewModel.cpp
    src/viewmodel/CueControlViewModel.cpp
    src/viewmodel/ScoreViewModel.cpp
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

# ============================================================================
# View 层（静态库，依赖 ViewModel + Qt Widgets）
# ============================================================================
add_library(snooker_view STATIC
    src/view/GameView.cpp
    src/view/CueControl.cpp
    src/view/ScoreBoard.cpp
    src/view/GameInfoPanel.cpp
)

target_include_directories(snooker_view PUBLIC
    ${CMAKE_SOURCE_DIR}/src/view
    ${CMAKE_SOURCE_DIR}/src/viewmodel
    ${CMAKE_SOURCE_DIR}/src/model
    ${CMAKE_SOURCE_DIR}/src/common
)

target_link_libraries(snooker_view PUBLIC
    snooker_viewmodel
    snooker_model
    snooker_common
    Qt6::Widgets
)

# ============================================================================
# App 层（可执行文件，依赖 View + Qt Widgets）
# ============================================================================
add_library(snooker_app STATIC
    src/app/MainWindow.cpp
)

target_include_directories(snooker_app PUBLIC
    ${CMAKE_SOURCE_DIR}/src/app
    ${CMAKE_SOURCE_DIR}/src/view
    ${CMAKE_SOURCE_DIR}/src/viewmodel
    ${CMAKE_SOURCE_DIR}/src/model
    ${CMAKE_SOURCE_DIR}/src/common
)

target_link_libraries(snooker_app PUBLIC
    snooker_view
    snooker_viewmodel
    snooker_model
    snooker_common
    Qt6::Widgets
)

# ============================================================================
# 主可执行文件
# ============================================================================
add_executable(Snooker2D
    src/main.cpp
)

target_link_libraries(Snooker2D PRIVATE
    snooker_app
    snooker_view
    snooker_viewmodel
    snooker_model
    snooker_common
    Qt6::Widgets
)
