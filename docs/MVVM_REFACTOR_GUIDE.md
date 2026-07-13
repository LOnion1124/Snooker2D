# 严格 MVVM 重构指南

本文面向后续执行代码重构的 agent，目标是把当前项目从“实用型 MVVM”推进到更严格的 MVVM 架构：

- View 层不包含 ViewModel 或 Model 头文件。
- View 层不持有具体 ViewModel 指针。
- View 和 ViewModel 的跨层通信全部通过 `common` 中定义的 Qt 信号/槽契约完成。
- App 层只负责对象创建、布局、依赖注入和启动，不承载游戏业务逻辑或 ViewModel 间数据同步。
- View 层只消费可展示状态，不解析游戏规则语义。

## 1. 目标依赖图

重构后的依赖方向应收敛为：

```text
common_core
  ├─ Constants / Types / MathUtils
  └─ 纯 C++，不依赖 Qt

common_contracts
  ├─ ViewState DTO
  ├─ ViewRequest / ViewEvent 契约
  └─ QtCore QObject 信号/槽接口

model
  └─ 依赖 common_core + QtCore

viewmodel
  └─ 依赖 model + common_core + common_contracts + QtCore

view
  └─ 依赖 common_contracts + common_core + QtWidgets

app
  └─ 依赖 view + viewmodel + model + common_contracts + QtWidgets
```

注意：现有 `snooker_common` 是纯 C++ 静态库，建议保留其纯净性，新增一个单独的 `snooker_contracts` target。如果课程或文档必须称为 common 层，可把文件放在 `src/common/contracts/`，但在 CMake 上单独建库。

## 2. 当前耦合点

执行重构前，先确认以下问题仍然存在：

- `src/view/*.cpp` 包含 `../viewmodel/*.h`。
- `src/view/*.h` 暴露 `setViewModel(ConcreteViewModel*)`。
- `GameView` 直接调用 `GameViewModel::ballPositions()`、`cueAngle()`、`setPower()`、`confirmWhiteBallPlacement()` 等方法。
- `MainWindow` 创建 `GameState` 后直接调用 `startNewGame()`。
- `MainWindow::setupBindings()` 负责角度同步、比分同步、犯规提示、比赛结束消息等业务协调。
- `GameView` 通过中文阶段字符串判断是否可瞄准或击球。
- `CMakeLists.txt` 中 `snooker_view` 依赖 `snooker_viewmodel` 和 `snooker_model`。

这些点都应在后续 task 中被移除。

## 3. Task 拆分

### Task 1：新增 common/contracts 契约层

目标：

- 在 common 层定义 View 与 ViewModel 通信所需的稳定契约。
- View 只依赖这些契约，不依赖具体 ViewModel 类型。

建议新增文件：

- `src/common/contracts/GameViewState.h`
- `src/common/contracts/GameUiBus.h`
- 可选：`src/common/contracts/ContractTypes.h`

建议 DTO：

```cpp
namespace Snooker2D {

struct BallViewState {
    double x = 0.0;
    double y = 0.0;
    int type = 0;
    bool pocketed = false;
    bool onTable = false;
};

struct TableViewState {
    QVector<BallViewState> balls;
    double cueAngle = 0.0;
    double cuePower = 50.0;
    bool centeredCoordinates = true;
    bool canAim = false;
    bool canShoot = false;
    bool isPlacingWhiteBall = false;
    bool isSimulating = false;
};

struct CueViewState {
    double angle = 0.0;
    double power = 50.0;
    double englishX = 0.0;
    double englishY = 0.0;
};

struct ScoreViewState {
    int player1Score = 0;
    int player2Score = 0;
    int player1Break = 0;
    int player2Break = 0;
    QString foulMessage;
    QString statusMessage;
};

struct GameInfoViewState {
    int currentPlayer = 0;
    QString phaseText;
    QString message;
    bool showWhiteBallPlacementHint = false;
};

} // namespace Snooker2D
```

建议 Bus：

```cpp
namespace Snooker2D {

class GameUiBus : public QObject {
    Q_OBJECT

public:
    explicit GameUiBus(QObject* parent = nullptr) : QObject(parent) {}

signals:
    // ViewModel -> View
    void tableStateChanged(const TableViewState& state);
    void cueStateChanged(const CueViewState& state);
    void scoreStateChanged(const ScoreViewState& state);
    void gameInfoStateChanged(const GameInfoViewState& state);
    void shotAnimationCancelled();

    // View -> ViewModel
    void cueAngleRequested(double angle);
    void cuePowerRequested(double power);
    void shotRequested();
    void shotAnimationFinished();
    void whiteBallPlacementRequested(double x, double y);
    void restartRequested();
};

} // namespace Snooker2D
```

实现要点：

- 对所有自定义 struct 添加 `Q_DECLARE_METATYPE`。
- 在应用启动或 Bus 构造阶段调用 `qRegisterMetaType<T>()`。
- DTO 只描述 UI 所需状态，不暴露 `GameState`、`Player`、`Ball` 指针。
- 若想降低一个大 Bus 的耦合，可拆成 `TableUiChannel`、`CueUiChannel`、`ScoreUiChannel`、`GameInfoUiChannel`。第一阶段建议先用一个 `GameUiBus`，迁移成本更低。

验收标准：

- `snooker_contracts` 可以独立编译。
- `src/common/MathUtils.*`、`Types.h`、`Constants.h` 不被迫依赖 Qt Widgets。
- DTO 中没有 Model/ViewModel 类指针。

### Task 2：调整 CMake 依赖边界

目标：

- 新增 contracts target。
- 让 View target 从编译层面无法依赖 ViewModel/Model。

建议调整：

- 新增 `snooker_contracts` 静态库或 INTERFACE 库。
- `snooker_contracts` 链接 `Qt6::Core` 和 `snooker_common`。
- `snooker_view` 只链接：
  - `snooker_contracts`
  - `snooker_common`
  - `Qt6::Widgets`
- 从 `snooker_view` 的 include directories 中移除：
  - `${CMAKE_SOURCE_DIR}/src/viewmodel`
  - `${CMAKE_SOURCE_DIR}/src/model`
- `snooker_viewmodel` 链接 `snooker_contracts`。

验收标准：

- `snooker_view` 不链接 `snooker_viewmodel`。
- `snooker_view` 不链接 `snooker_model`。
- 删除 View 中所有 ViewModel include 后仍可编译。

### Task 3：重构 View 层绑定接口

目标：

- View 只绑定 `GameUiBus` 或 common channel。
- View 只发请求信号，不直接调用 ViewModel command。
- View 只消费 ViewState，不调用 ViewModel getter。

建议逐个迁移。

#### 3.1 GameView

当前问题：

- 持有 `GameViewModel*`。
- 在 `refresh()` 中主动读取 VM getter。
- 在鼠标/滚轮事件中直接调用 `setAngle()`、`setPower()`、`confirmWhiteBallPlacement()`。

建议改法：

- `setViewModel(GameViewModel*)` 改为 `bind(GameUiBus*)`。
- 成员 `GameViewModel* m_viewModel` 改为 `GameUiBus* m_bus`。
- 新增 slot：`void applyTableState(const TableViewState& state);`
- `applyTableState()` 更新：
  - `m_cachedBallPositions` 或改为 `QVector<BallViewState>`
  - `m_cachedCueAngle`
  - `m_cachedCuePower`
  - `m_cachedCanAim`
  - `m_cachedCanShoot`
  - `m_cachedIsPlacingWhiteBall`
  - `m_centeredCoordinates`
- `wheelEvent()` 改为 `emit`/调用 bus 信号：`cuePowerRequested(newPower)`。
- `updateCueAngleFromMouse()` 改为发 `cueAngleRequested(angle)`。
- `tryPlaceWhiteBall()` 改为发 `whiteBallPlacementRequested(x, y)`。
- `shotAnimationFinished()` 可保留 View 自身信号，但由 `bind()` 内连接到 bus 的 `shotAnimationFinished()`，或直接发 bus 信号。

业务状态要求：

- `aimingToolsVisible()` 不再解析 `gamePhase` 文本。
- 由 `TableViewState::canAim`、`canShoot`、`isSimulating` 决定显示和交互。

验收标准：

- `GameView.h/.cpp` 不包含 `GameViewModel`。
- `GameView` 不调用任何 ViewModel 方法。
- `GameView` 不使用中文阶段字符串判断业务状态。

#### 3.2 CueControl

当前问题：

- 持有 `CueControlViewModel*`。
- 直接连接 `CueControlViewModel::angleChanged` 和 `powerChanged`。

建议改法：

- `setViewModel(CueControlViewModel*)` 改为 `bind(GameUiBus*)`。
- 监听 `cueStateChanged(const CueViewState&)` 更新 slider 和 label。
- slider 若未来允许用户操作，应发：
  - `cueAngleRequested(double)`
  - `cuePowerRequested(double)`
- 当前 slider 是 disabled，可以先仅作为状态显示。

验收标准：

- `CueControl` 不包含 `CueControlViewModel.h`。
- `CueControl` 不持有具体 VM 指针。

#### 3.3 ScoreBoard

当前问题：

- 持有 `ScoreViewModel*`。
- `refresh()` 主动读取 VM getter。

建议改法：

- 改为 `bind(GameUiBus*)`。
- 监听 `scoreStateChanged(const ScoreViewState&)`。
- 删除 `m_viewModel`。
- `refresh()` 可改成 `applyScoreState(const ScoreViewState&)`。

验收标准：

- `ScoreBoard` 不包含 `ScoreViewModel.h`。
- `ScoreBoard` 不主动查询 VM。

#### 3.4 GameInfoPanel

当前问题：

- 持有 `GameViewModel*`。
- restart button 直接调用 `GameViewModel::restartGame()`。
- 通过 `gamePhase()`、`currentPlayer()`、`isPlacingWhiteBall()` getter 更新 UI。

建议改法：

- 改为 `bind(GameUiBus*)`。
- 监听 `gameInfoStateChanged(const GameInfoViewState&)`。
- restart button 发 `restartRequested()`。
- `phaseStyleSheet()` 可以保留，因为它是展示样式映射；但更严格时可由 VM 提供 `phaseKind` 枚举，View 根据枚举选样式。

验收标准：

- `GameInfoPanel` 不包含 `GameViewModel.h`。
- restart 不直接调用 VM。

### Task 4：新增或改造 ViewModel 协调器

目标：

- 把 `MainWindow::setupBindings()` 中的业务同步移动到 ViewModel 层。
- ViewModel 统一绑定 `GameUiBus`，接收 View 请求并推送 ViewState。

推荐新增：

- `src/viewmodel/GameSessionViewModel.h`
- `src/viewmodel/GameSessionViewModel.cpp`

推荐职责：

- 创建或持有 `GameState`，或接收外部注入的 `GameState*`。
- 持有现有 `GameViewModel`、`CueControlViewModel`、`ScoreViewModel`，作为内部子 VM。
- 连接子 VM 和 Model 的信号。
- 统一向 `GameUiBus` 推送：
  - `TableViewState`
  - `CueViewState`
  - `ScoreViewState`
  - `GameInfoViewState`
- 统一处理来自 `GameUiBus` 的请求：
  - `cueAngleRequested`
  - `cuePowerRequested`
  - `shotAnimationFinished`
  - `whiteBallPlacementRequested`
  - `restartRequested`

推荐第一阶段保留现有三个 VM，但不再让 App 直接协调它们：

```text
GameSessionViewModel
  ├─ GameState
  ├─ GameViewModel
  ├─ CueControlViewModel
  └─ ScoreViewModel
```

迁移策略：

- 先把 `MainWindow::setupBindings()` 的连接原样搬到 `GameSessionViewModel::setupBindings()`。
- 再逐步把“从 getter 取值再 set 到另一个 VM”的同步改为内部方法。
- 最后可以评估是否删除 `ScoreViewModel`、`CueControlViewModel`，把它们并入 `GameSessionViewModel`。

启动逻辑：

- `GameState::startNewGame()` 不应由 `MainWindow` 直接调用。
- `GameSessionViewModel` 提供 `start()` 或构造后自行启动。
- `restartRequested()` 进入 `GameSessionViewModel::restartGame()`，再操作 Model/子 VM。

验收标准：

- `MainWindow` 中没有比分同步、角度双向同步、犯规提示、比赛结束消息等业务连接。
- `MainWindow` 不直接调用 `GameState::startNewGame()`。
- ViewModel 层是唯一了解 Model 细节的上层。

### Task 5：收敛 GameViewModel 输出状态

目标：

- ViewModel 输出 UI 可直接消费的结构化状态。
- View 不需要从 `gamePhase` 字符串推导规则。

建议在 `GameViewModel` 或 `GameSessionViewModel` 中维护：

- `bool canAim`
- `bool canShoot`
- `bool isSimulating`
- `bool isPlacingWhiteBall`
- `bool isGameOver`
- `QString phaseText`

状态规则示例：

```cpp
bool canAim = !isPlacingWhiteBall
           && !isSimulating
           && phase != GamePhase::NotStarted
           && phase != GamePhase::GameOver;

bool canShoot = canAim && cueBallIsOnTable;
```

注意：

- `phaseText` 是展示文案，不应作为 View 判断逻辑的依据。
- 如果 View 需要样式分支，优先提供 `GamePhase` 或新的 `PhaseKind` 枚举，而不是解析中文字符串。

验收标准：

- View 层不存在 `contains("模拟中")`、`"未开始"`、`"比赛结束"` 这类业务判断。
- View 的交互启用/禁用来自 ViewState 布尔字段。

### Task 6：简化 App 层

目标：

- `MainWindow` 成为 composition root。
- 不再处理游戏业务、不再转发 ViewModel 内部状态。

建议 `MainWindow` 最终形态：

```cpp
void MainWindow::initGame() {
    m_uiBus = new GameUiBus(this);
    m_sessionViewModel = new GameSessionViewModel(m_uiBus, this);

    m_gameView->bind(m_uiBus);
    m_cueControl->bind(m_uiBus);
    m_scoreBoard->bind(m_uiBus);
    m_gameInfoPanel->bind(m_uiBus);

    m_sessionViewModel->start();
}
```

`MainWindow` 可以保留：

- `setupUI()`
- `initGame()`
- 必要的窗口配置

`MainWindow` 应删除：

- `setupBindings()` 中所有业务连接
- `GameState* m_gameState`
- 具体子 VM 指针，如果它们只由 `GameSessionViewModel` 内部使用

验收标准：

- `MainWindow.cpp` 中不出现 `performShot`、`startNewGame`、`setAngle`、`setPower`、`setPlayerScore` 等业务调用。
- `MainWindow` 不读取任何 ViewModel getter。

### Task 7：清理旧接口和文档

目标：

- 删除旧的跨层接口，防止新旧架构并存。

清理项：

- 删除 View 中的 `setViewModel(...)`。
- 删除 View 头文件中的具体 ViewModel 前置声明。
- 删除 View 源文件中的 `../viewmodel/*.h` include。
- 删除 CMake 中 View 对 ViewModel/Model 的 include path 和 link。
- 更新 `docs/DESIGN.md` 和 `docs/app/design.md`，说明新依赖图和 `GameUiBus` 契约。

验收标准：

- `rg "viewmodel" src/view` 无结果。
- `rg "../model|../viewmodel" src/view` 无结果。
- `rg "setViewModel" src/view src/app` 无结果，除非保留了过渡兼容层；最终版本不应保留。

### Task 8：增加架构护栏测试

目标：

- 防止后续改动重新引入层间依赖。

建议新增脚本：

- `tests/architecture/check_mvvm_boundaries.ps1`

检查项：

```powershell
$violations = @()

$violations += rg '../viewmodel|../model|viewmodel/|model/' src/view
$violations += rg 'setViewModel' src/view src/app
$violations += rg 'snooker_viewmodel|snooker_model' CMakeLists.txt
```

实际脚本应区分合法位置和非法位置，例如 `CMakeLists.txt` 中 app/viewmodel 依赖 model 是合法的，只有 `snooker_view` 的 target 配置中出现才非法。

也可新增一个 CMake custom target：

```cmake
add_custom_target(check_architecture
    COMMAND powershell -ExecutionPolicy Bypass
            -File ${CMAKE_SOURCE_DIR}/tests/architecture/check_mvvm_boundaries.ps1
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
```

验收标准：

- 运行架构检查通过。
- 若在 `src/view` 中新增 `#include "../viewmodel/GameViewModel.h"`，检查能失败。

## 4. 推荐执行顺序

建议按以下顺序提交，降低一次性改动风险：

1. 新增 `common/contracts` 和 CMake target，但暂不迁移 View。
2. 新增 `GameSessionViewModel`，先复用现有 VM 和现有业务连接。
3. 迁移 `ScoreBoard` 到 Bus。
4. 迁移 `CueControl` 到 Bus。
5. 迁移 `GameInfoPanel` 到 Bus。
6. 迁移 `GameView` 到 Bus，这是风险最高的一步。
7. 删除 `MainWindow::setupBindings()` 业务连接。
8. 收紧 CMake，让 `snooker_view` 不再依赖 VM/Model。
9. 删除旧 `setViewModel` 接口。
10. 添加架构护栏脚本并更新文档。

其中 `GameView` 牵涉绘制、动画、鼠标、滚轮、白球放置，建议单独提交。

## 5. 回归测试清单

每个迁移阶段至少验证：

- 项目能完整编译。
- 启动后球桌正常绘制。
- 开局阶段当前玩家和阶段显示正确。
- 鼠标移动能改变瞄准角。
- 滚轮能改变力度。
- 点击击球会先播放球杆动画，再进入物理模拟。
- 模拟过程中瞄准工具隐藏或锁定。
- 球停止后比分、阶段、当前玩家刷新。
- 白球落袋后显示 D 区放置提示。
- 点击 D 区有效位置能放回白球。
- 重启按钮能重置球、比分、提示、角度和力度。

## 6. Agent 注意事项

- 不要在 View 中引入任何 ViewModel 或 Model 头文件。
- 不要用 `QObject::property()` 字符串反射绕过类型依赖，这会让错误从编译期推迟到运行期。
- 不要让 `GameUiBus` 包含业务逻辑；它只是通信契约和信号载体。
- 不要把 Model 指针塞进 ViewState。
- 不要让 App 层通过 lambda 继续做比分、阶段、犯规、击球协调。
- 不要在同一提交里同时大改物理/规则逻辑和 MVVM 边界，除非是编译必须。
- 如果必须保留过渡接口，需在注释中标记 TODO，并在最终清理 task 中删除。

## 7. 最终验收标准

最终架构完成时，应满足：

```text
src/view
  不 include src/viewmodel
  不 include src/model
  不出现具体 ViewModel 类型
  不主动读取 ViewModel getter
  不直接调用 ViewModel command

src/app
  不承载游戏业务逻辑
  不做 ViewModel 间数据同步
  只创建对象、组装 UI、绑定 common contracts

src/viewmodel
  是 View 请求进入 Model 的唯一上层入口
  是 Model 状态转换为 ViewState 的唯一位置

src/common/contracts
  是 View 和 ViewModel 的唯一跨层通信契约
```

完成后，MVVM 边界应从“约定上分层”变为“编译依赖上强制分层”。
