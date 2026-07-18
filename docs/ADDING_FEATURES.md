# 添加功能指南

本文档说明如何在当前 MVVM 架构中添加新功能，面向后续接手开发的开发者。

## 架构速览

在开始之前，先理解四个硬约束：

1. **View 不链接 ViewModel/Model**（CMake 编译期强制）
2. **ViewModel 不链接 View**（不 include 任何 View 头文件）
3. **Common 层纯 C++**（除 `contracts/GameViewState.h` 含 Qt 类型外，其余不依赖 Qt）
4. **所有跨层 connect 在 `App.cpp` 中完成**（View 和 ViewModel 互不感知）

```
Common（DTO / 类型 / 常量 / 工具函数）
  ↑           ↑              ↑
View       ViewModel        Model
(slots/    (slots/         (signals)
 signals)   signals)
  │           │              │
  └───────────┼──────────────┘
              │ 全部 connect 在 App.cpp
           App 层
```

## 添加功能的四种典型场景

### 场景 A：ViewModel 给 View 推送新数据

例如：需要在计分板上显示"当前连续进球数"。

**Step 1 — 扩展 DTO**（`src/common/contracts/GameViewState.h`）

在对应的 ViewState 结构体中加字段：

```cpp
struct ScoreViewState {
    // ... 已有字段 ...
    int consecutivePots = 0;  // 新增：连续进球数
};
```

**Step 2 — ViewModel 填充数据**（`src/viewmodel/GameSessionViewModel.cpp`）

在对应的 `pushXxxState()` 中从 Model 读数并写入 DTO：

```cpp
void GameSessionViewModel::pushScoreState() {
    ScoreViewState state;
    // ... 已有逻辑 ...
    state.consecutivePots = m_gameState->consecutivePots();  // 新增
    emit scoreStateReady(state);
}
```

**Step 3 — View 消费数据**（`src/view/ScoreBoard.cpp`）

在 `applyScoreState()` 槽中更新对应控件：

```cpp
void ScoreBoard::applyScoreState(const ScoreViewState& state) {
    // ... 已有逻辑 ...
    m_consecutiveLabel->setText(
        QStringLiteral("连续进球: %1").arg(state.consecutivePots));  // 新增
}
```

**关键点**：
- 不需要修改 App.cpp（信号槽连接不变）
- 不需要修改 ViewModel 头文件（信号签名不变，DTO 变了会自动跟着变）
- View 层新增的 Label 在 `setupUI()` 中创建

---

### 场景 B：View 新增用户交互

例如：加一个"撤销上一杆"按钮。

**Step 1 — View 声明信号**（`src/view/GameInfoPanel.h`）

```cpp
class GameInfoPanel : public QWidget {
    Q_OBJECT
signals:
    void restartRequested();
    void undoRequested();  // 新增信号
};
```

**Step 2 — View 发射信号**（`src/view/GameInfoPanel.cpp`）

在按钮的 clicked 信号中转发：

```cpp
// setupUI() 中
connect(m_undoButton, &QPushButton::clicked,
        this, &GameInfoPanel::undoRequested);
```

**Step 3 — ViewModel 声明槽**（`src/viewmodel/GameSessionViewModel.h`）

```cpp
public slots:
    void restart();
    void undoLastShot();  // 新增槽
```

**Step 4 — ViewModel 实现槽**（`src/viewmodel/GameSessionViewModel.cpp`）

```cpp
void GameSessionViewModel::undoLastShot() {
    if (m_gameState) {
        m_gameState->undoLastShot();
        pushAllStates();  // 刷新 View
    }
}
```

**Step 5 — App 层连接**（`src/app/App.cpp`）

```cpp
// View → ViewModel（命令绑定）区域新增一行：
QObject::connect(gameInfoPanel, &GameInfoPanel::undoRequested,
        &sessionViewModel, &GameSessionViewModel::undoLastShot);
```

**关键点**：
- App.cpp 只加一条 connect，不改其他代码
- View 只声明信号和发射，不关心谁处理
- ViewModel 只声明槽和处理逻辑，不关心谁触发

---

### 场景 C：Model 新增状态变更通知

例如：新增"超时警告"事件。

**Step 1 — Model 声明信号**（`src/model/GameState.h`）

```cpp
class GameState : public QObject {
    Q_OBJECT
signals:
    void phaseChanged(GamePhase phase);
    void timeWarning(int secondsLeft);  // 新增信号
};
```

**Step 2 — Model 发射信号**（`src/model/GameState.cpp`）

在合适的时机 emit，例如在 `updateSimulation()` 或新增的计时逻辑中。

**Step 3 — ViewModel 连接信号**（`src/viewmodel/GameSessionViewModel.cpp` 构造函数）

```cpp
// 通知绑定区域新增：
connect(m_gameState, &GameState::timeWarning,
        this, &GameSessionViewModel::onTimeWarning);
```

**Step 4 — ViewModel 处理**（`src/viewmodel/GameSessionViewModel.cpp`）

```cpp
void GameSessionViewModel::onTimeWarning(int secondsLeft) {
    m_statusMessage = QStringLiteral("剩余 %1 秒").arg(secondsLeft);
    pushGameInfoState();  // 推送到 View
}
```

**关键点**：
- Model → ViewModel 的 connect 放在 ViewModel 构造函数内，不动 App.cpp
- 这是"通知绑定"，因为 ViewModel 本就持有 Model 指针，它们是翻译关系
- 如需在 View 上展示，走场景 A 的流程（扩 DTO → push → apply）

---

### 场景 D：新增整个 View 组件

例如：新增一个"历史记录面板"。

**Step 1 — 创建 View 文件**（`src/view/HistoryPanel.h/.cpp`）

```cpp
// HistoryPanel.h
#pragma once
#include <QWidget>
#include "contracts/GameViewState.h"  // 如需新 DTO，先扩

namespace Snooker2D {

class HistoryPanel : public QWidget {
    Q_OBJECT
public:
    explicit HistoryPanel(QWidget* parent = nullptr);

public slots:
    void applyHistoryState(const HistoryViewState& state);  // 纯接收
};
} // namespace Snooker2D
```

**Step 2 — 注册到 CMake**（`CMakeLists.txt`）

在 `snooker_view` 的 `add_library` 中加入新文件：

```cmake
add_library(snooker_view STATIC
    src/view/GameView.cpp
    # ... 已有文件 ...
    src/view/HistoryPanel.cpp   # 新增
)
```

**Step 3 — 挂载到 MainWindow**（`src/view/MainWindow.h` + `.cpp`）

```cpp
// MainWindow.h
class HistoryPanel;
// ...
HistoryPanel* historyPanel() const { return m_historyPanel; }

// MainWindow.cpp setupUI() 中创建并加入布局
m_historyPanel = new HistoryPanel(this);
rightLayout->addWidget(m_historyPanel);
```

**Step 4 — 如需新 DTO**，走场景 A 的 Step 1-2，在 `GameViewState.h` 中新增 DTO 结构体并在 ViewModel 中填充。

**Step 5 — App 层连接**（`src/app/App.cpp`）

```cpp
HistoryPanel* historyPanel = mainWindow.historyPanel();
QObject::connect(&sessionViewModel, &GameSessionViewModel::historyStateReady,
        historyPanel, &HistoryPanel::applyHistoryState);
```

**关键点**：
- 新 View 只依赖 `snooker_common` + `Qt::Widgets`，不碰 ViewModel/Model
- MainWindow 暴露 getter，App 层拿指针做 connect
- 和现有 4 个 View 的模式完全一致

---

## 常见错误

### ❌ 错误 1：在 View 中 include ViewModel 头文件

```cpp
// GameView.cpp — 错误示范
#include "GameSessionViewModel.h"  // ❌ 编译期就会被 CMake 拦住
```

**正确做法**：View 只通过信号/槽通信，不知道 ViewModel 的存在。如果 View 需要某个值，应该是 ViewModel 通过 DTO 推送过来。

### ❌ 错误 2：在 ViewModel 中 include View 头文件

```cpp
// GameSessionViewModel.h — 错误示范
#include "GameView.h"  // ❌ 破坏单向依赖
```

**正确做法**：ViewModel 只声明自己的信号和槽，不持有任何 View 引用。

### ❌ 错误 3：在 Common 层放 Qt Widgets 依赖

```cpp
// common/Types.h — 错误示范
#include <QWidget>  // ❌ Common 层要保持纯 C++（contracts/ 除外）
```

**正确做法**：需要 Qt Widgets 类型的定义放在 View 层或 App 层。

### ❌ 错误 4：connect 写在 View 或 ViewModel 内部

```cpp
// GameView.cpp — 错误示范
connect(this, &GameView::angleChanged,
        someViewModel, &GameSessionViewModel::setAngle);  // ❌ View 不知道 ViewModel
```

**正确做法**：跨层 connect 全部在 `App.cpp` 中完成。View 内部只做控件级 connect（如按钮 → 自身信号转发）。

### ❌ 错误 5：忘了 `qRegisterMetaType`

新增 DTO 如果在 queued connection 中使用（跨线程），需要在 `main.cpp` 中注册：

```cpp
qRegisterMetaType<Snooker2D::NewViewState>("Snooker2D::NewViewState");
```

如果 App.cpp 中全部是直接连接（同线程），可以省略。

---

## 代码风格速查

- 注释用中文，不用英文
- 不用 Doxygen（`///` `@param` `@return`）
- 不用 ASCII 分隔线注释（`// ====`）
- 头文件保护用 `#pragma once`
- 文件末尾留一个空行
- 命名空间：`Snooker2D`
- 成员变量前缀 `m_`，如 `m_gameState`

---

## 验证清单

添加功能后，逐项确认：

- [ ] `cmake --build build` 编译通过
- [ ] `cmake --build build --target check_architecture` 架构护栏通过
- [ ] `build/test_mathutils.exe` 已有测试全部通过
- [ ] 手动运行 exe，新功能行为正确
- [ ] 新代码风格与现有代码一致（中文注释、无分隔线、`m_` 前缀）
- [ ] 如有新信号/槽，已在上述四种场景的对应文件中声明和连接
