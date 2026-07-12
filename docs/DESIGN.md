# 架构设计文档

## 1. 架构选型

采用课程讲授的 **MVVM 五层架构**（Common / Model / ViewModel / View / App），每层编译为独立静态库。

**选型理由**：
- MVVM 是 MVC→MVP→MVVM 演进链条的终点，符合"最先进框架"要求
- 五层拆分支持三人独立开发，通过接口约定并行推进
- Model 层纯 C++（仅依赖 Qt Core 的 QObject 信号机制），可脱离界面做单元测试

## 2. 各层职责

| 层 | 职责 | 依赖 |
|----|------|------|
| Common | 全局类型、常量、数学工具函数 | 无 |
| Model | 游戏状态机、物理引擎、规则判定 | Common + Qt::Core |
| ViewModel | 数据转换、属性暴露、命令绑定 | Model + Common + Qt::Core |
| View | 界面渲染、用户输入 | ViewModel + Qt::Widgets |
| App | 对象创建、依赖注入、启动配置 | 所有层 + Qt::Widgets |

## 3. 关键设计决策

### 3.1 FoulResult 位置

`FoulResult` 定义在 `src/model/Rules.h` 而非 `src/common/Types.h`。理由：该结构体包含 `QString description`（Qt 类型），不适合放入纯 C++ 的 Common 层。ViewModel 通过前置声明引用。

### 3.2 袋口坐标位置

袋口坐标定义在 `src/model/Table.h`（`Pocket` 结构体）而非 Constants。理由：袋口位置是球桌几何的一部分，由 Table 类统一管理，Constants 仅存放无上下文的标量。

### 3.3 vcpkg 不使用

Qt 通过 aqtinstall 预编译安装，CMake 以 `find_package` 解析。vcpkg 的 MinGW triplet 会触发源码编译（耗时过长），故 `vcpkg.json` 依赖项留空。

### 3.4 ViewModel 拆分策略

GameViewModel 为总控 VM（持有 GameState），CueControlViewModel 和 ScoreViewModel 为卫星 VM。App 层在 MainWindow 中负责三者之间的信号转发，ViewModel 之间不直接耦合。

## 4. 层间接口约定

详见 `TASK_ALLOCATION.md` 第 5 节。
