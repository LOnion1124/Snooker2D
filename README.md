# Snooker2D

基于 C++17 和 Qt6 的 2D 斯诺克台球游戏，采用 MVVM 四层架构，三人协作开发。

## 快速开始

### 环境要求

- Windows 10/11
- MinGW-w64 (GCC 14.2) 或 MSVC 2022
- CMake 3.20+
- Ninja
- Qt 6.x（通过 aqtinstall 或 MSYS2 安装）

> 详细安装步骤见 `docs/DEV_SETUP.md`

### 构建

```bash
# MinGW
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

# 运行
./build/Snooker2D.exe
```

### 运行测试

```bash
./build/test_mathutils.exe    # Common 层单元测试
./build/test_physics.exe      # 物理引擎测试
./build/test_gamestate.exe    # 游戏状态机测试
```

### 架构检查

```bash
cmake --build build --target check_architecture
```

## 架构

```
Common（类型 / 常量 / 工具函数 / DTO）
  ↑           ↑              ↑
View       ViewModel        Model
(slots/    (slots/         (signals)
 signals)   signals)
  │           │              │
  └───────────┼──────────────┘
              │ 全部跨层 connect 在 App.cpp
           App 层
```

| 层 | 职责 | 编译依赖 |
|----|------|----------|
| Common | 全局类型、常量、数学工具、ViewState DTO | Qt::Core |
| Model | 游戏状态机、物理引擎、规则判定 | Common + Qt::Core |
| ViewModel | 数据翻译、状态推送、命令处理 | Model + Common |
| View | 界面渲染、鼠标交互 | Common + Qt::Widgets |
| App | 对象装配、跨层信号槽绑定 | 所有层 |

关键约束：**View 层不链接 ViewModel/Model**，由 CMake 编译期强制。

## 通信机制

三层绑定，各定其位：

| 绑定类型 | 方向 | 连接位置 | 数量 |
|----------|------|----------|:--:|
| 属性绑定 | ViewModel → View | App.cpp | 6 条 |
| 命令绑定 | View → ViewModel | App.cpp | 6 条 |
| 通知绑定 | Model → ViewModel | ViewModel 构造 | 9 条 |

## 项目结构

```
Snooker2D/
├── src/
│   ├── common/          # Common 层
│   │   ├── Constants.h
│   │   ├── Types.h
│   │   ├── MathUtils.h/.cpp
│   │   └── contracts/
│   │       └── GameViewState.h   # DTO 定义
│   ├── model/           # Model 层
│   │   ├── Ball.h/.cpp
│   │   ├── Table.h/.cpp
│   │   ├── Physics.h/.cpp
│   │   ├── GameState.h/.cpp
│   │   ├── Player.h/.cpp
│   │   └── Rules.h/.cpp
│   ├── viewmodel/       # ViewModel 层
│   │   └── GameSessionViewModel.h/.cpp
│   ├── view/            # View 层
│   │   ├── MainWindow.h/.cpp
│   │   ├── GameView.h/.cpp
│   │   ├── CueControl.h/.cpp
│   │   ├── EnglishControlPanel.h/.cpp
│   │   ├── ScoreBoard.h/.cpp
│   │   ├── GameInfoPanel.h/.cpp
│   │   ├── GameControlPanel.h/.cpp
│   │   └── UiLanguage.h/.cpp
│   └── app/             # App 层
│       ├── App.h/.cpp
│       └── main.cpp
├── tests/
│   ├── test_mathutils.cpp
│   ├── test_physics.cpp
│   ├── test_gamestate.cpp
│   └── architecture/
│       └── check_mvvm_boundaries.ps1
├── docs/
│   ├── ARCHITECTURE.md           # 架构文档
│   ├── MODEL_VIEWMODEL.md        # Model/ViewModel 层详解
│   ├── DEV_SETUP.md              # 环境搭建指南
│   ├── ADDING_FEATURES.md        # 添加功能指南
│   ├── FEATURE_ENGLISH.md        # 加塞功能接口文档
│   ├── NOTES.md                  # 开发日志
│   └── TASK_ALLOCATION.md        # 分工表
├── .github/workflows/
│   └── ci.yml                    # GitHub Actions CI
└── CMakeLists.txt
```

## CI/CD

push 到 main 分支自动触发 GitHub Actions，包含：编译 → 跑 86 条测试 → 架构护栏检查。

[![Build and Test](https://github.com/LOnion1124/Snooker2D/actions/workflows/ci.yml/badge.svg)](https://github.com/LOnion1124/Snooker2D/actions/workflows/ci.yml)

## 已完成功能

- 斯诺克标准 22 颗球初始摆球和球桌渲染
- 鼠标瞄准、击球（含球杆动画）
- 力度/角度双模控制（滑块 + 鼠标拖动）
- 球-球弹性碰撞、球-库边反弹、摩擦力减速
- 袋口检测与落袋、D 区白球复位
- 加塞系统（上/下/左/右塞，9 方向）
- 犯规判罚（白球落袋、空杆、先击错球、无球碰库、彩球落袋）
- 计分板、游戏信息面板、中英文切换
- 86 条单元测试、架构护栏自动检查

## 团队

| 角色 | 成员 | 负责 |
|------|------|------|
| 开发者 A | 鲁亦智 | View 层 |
| 开发者 B | 何广一 | Model + ViewModel 层 |
| 开发者 C | 井淳 | Common + App 层、构建配置、CI/CD |

## 开发文档

新功能开发前请阅读 `docs/ADDING_FEATURES.md`，了解如何在不同层添加代码和信号槽连接。
