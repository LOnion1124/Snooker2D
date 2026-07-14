# 架构设计文档

## 1. 架构选型

采用课程讲授的 **MVVM 五层架构**，每层编译为独立静态库。

**选型理由**：
- MVVM 是 MVC→MVP→MVVM 演进链条的终点，符合"最先进框架"要求
- 五层拆分支持三人独立开发，通过 contracts 契约并行推进
- Model 层纯 C++（仅依赖 Qt Core 的 QObject 信号），可脱离界面做单元测试
- View 层通过编译期强制隔离，无法依赖 ViewModel/Model，架构违规直接编译报错

## 2. 各层职责与依赖

| 层 | 职责 | 编译依赖 |
|----|------|----------|
| Common | 全局类型、常量、数学工具 | 无（纯 C++） |
| Contracts | View/ViewModel 通信契约（DTO + Bus） | Common + Qt::Core |
| Model | 游戏状态机、物理引擎、规则判定 | Common + Qt::Core |
| ViewModel | 数据转换、GameSessionViewModel 协调 | Model + Contracts + Qt::Core |
| View | 界面渲染、用户输入 | Contracts + Common + Qt::Widgets |
| App | 对象创建、Bus 注入、启动 | ViewModel + View + Qt::Widgets |

关键约束：**View 层不链接 ViewModel/Model**。编译依赖由 CMake 强制执行，架构护栏脚本（`check_architecture` target）持续验证。

## 3. 通信机制

### 数据方向（ViewModel → View）

GameSessionViewModel 将 Model 数据转换为 ViewState DTO，通过 GameUiBus 信号推送到 View：

```
Model 信号 → GameSessionViewModel::pushXxxState()
           → emit m_bus->xxxStateChanged(dto)
           → View::applyXxxState(dto)
```

View 被动接收状态快照，不主动查询 ViewModel。

### 命令方向（View → ViewModel）

View 发出请求信号到 Bus，GameSessionViewModel 监听并处理：

```
View 用户操作 → emit m_bus->xxxRequested(...)
             → GameSessionViewModel::onXxxRequested()
             → 调子 ViewModel / Model
```

### GameUiBus

定义在 `src/common/contracts/GameUiBus.h`，是 View 与 ViewModel 之间的唯一通信契约。12 条信号，不含业务逻辑。

## 4. 关键设计决策

### 4.1 contracts 层独立于 Common 层

Common 层（Constants/Types/MathUtils）保持纯 C++、不依赖 Qt。contracts 层（GameViewState/GameUiBus）依赖 Qt::Core，单独编译为 `snooker_contracts`。

### 4.2 GameSessionViewModel 协调器

ViewModel 层新增 GameSessionViewModel，统一处理：
- 创建并持有 Bus、Model、所有子 ViewModel
- 接收 Bus 上的 View 请求 → 转发到子 ViewModel/Model
- 监听 Model 信号 → 转换为 ViewState → 推送到 Bus

App 层不再做任何业务协调。

### 4.3 View 不解析游戏规则

View 通过 ViewState 中的 `canAim`/`canShoot`/`isSimulating`/`phaseKind` 等布尔/枚举字段判断交互状态，不通过中文字符串（如 `contains("模拟中")`）做业务分支。

### 4.4 FoulResult 在 Model 层

`FoulResult` 定义在 `src/model/Rules.h`，含 `QString description`（Qt 类型），不适合纯 C++ 的 Common 层。

### 4.5 袋口坐标在 Table.h

袋口坐标（`Pocket` 结构体）由 Model 层 `Table` 类统一管理。

### 4.6 vcpkg 不使用

Qt 通过 aqtinstall 预编译安装，CMake 以 `find_package` 解析。

## 5. App 层组装流程

```
MainWindow():
  setupUI()           // 创建 4 个 View 控件
  registerContractTypes()
  initGame():
    new GameSessionViewModel     // 内部创建 Bus + Model + 子 VM
    m_uiBus = vm->bus()
    view->bind(m_uiBus)          // 4 个 View 统一绑定同一 Bus
    vm->start()
```
