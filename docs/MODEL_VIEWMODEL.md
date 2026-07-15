# Model 与 ViewModel 层讲解

## Model 层

### 定位

纯游戏逻辑层，继承 QObject 仅为了利用 Qt 信号/槽通知状态变更。不依赖任何 UI 概念，不包含 Qt Widgets。

### 核心类

| 类 | 文件 | 职责 |
|----|------|------|
| `Ball` | `src/model/Ball.h/.cpp` | 球的位置、速度、落袋状态、复位能力 |
| `Table` | `src/model/Table.h/.cpp` | 球桌尺寸、袋口位置定义 |
| `Physics` | `src/model/Physics.h/.cpp` | 物理引擎：运动积分、弹性碰撞、摩擦衰减、袋口检测、边界约束 |
| `GameState` | `src/model/GameState.h/.cpp` | 状态机：开球、击球、阶段转换、白球放置、比赛结束 |
| `Player` | `src/model/Player.h/.cpp` | 玩家姓名、分数、单杆成绩 |
| `Rules` | `src/model/Rules.h/.cpp` | 犯规检测、罚分计算、目标球判定 |

### 关键设计

**坐标系统**：球桌采用中心坐标系，`(0, 0)` 为台面中心，范围 `[-400, -200]` 到 `[400, 200]`。View 通过 `centeredCoordinates` 标志在绘制时偏移到左上角原点。

**物理流水线**（`Physics::step()`）：

```
moveBalls → checkBallBallCollisions → checkBallCushionCollisions
  → checkPocketDetection → applyFriction → applyHardConstraint → checkPullBackBalls
```

每层处理上一层遗留的问题：
- 碰撞响应：位置分离 + 弹性速度交换，球-球恢复系数 0.88，球-库边 0.77
- 硬约束：球心不得超出台面边界 +5px，防止链式碰撞推出边界
- 慢球回拉：仅对速度 < 0.1 的球生效，将球心拉回台面内，避免慢球卡在库边外

**彩球复位**：红球未清空前，彩球落袋后立即调用 `Ball::respot()` 回到 `m_initialPosition`，这是斯诺克规则的核心要求。

**D 区放置**：白球落袋后，对局进入 WhiteBallPlacing 阶段。`isValidPlacement()` 检查 x ≤ BAULK_LINE_X 且在 D 半圆内、不与任何在台球重叠。

### 数据流

```
View 点击 → ViewModel::onShotAnimationFinished()
  → GameState::performShot(angle, power)    // 白球获得初速度
  → emit simulationStarted
  → [每帧] ViewModel::onSimulationTick
    → GameState::updateSimulation()
      → Physics::step()
  → allBallsStopped
    → emit simulationFinished
    → GameState::handlePostShot()            // 计分、犯规、阶段转换、彩球复位
```

### 关键代码接口

```cpp
// GameState — 对外核心方法
void performShot(double angle, double power);
void updateSimulation();          // 每帧调用
bool isSimulationRunning() const;
void placeWhiteBall(Vector2D pos); // D 区放置

// Physics — 流水线
void step(double deltaTime, std::vector<Ball*>& balls, const Table& table);
static bool allBallsStopped(const std::vector<Ball*>& balls);
```

---

## ViewModel 层

### 定位

Model 与 View 之间唯一的桥梁。将 Model 的 `std::vector<unique_ptr<Ball>>` 映射为 View 可消费的扁平 DTO，将 View 的用户操作翻译为 Model 指令。

### 唯一类

`GameSessionViewModel`（`src/viewmodel/GameSessionViewModel.h/.cpp`，约 317 行）

### 信号架构

不依赖 Bus 或中介对象。直接声明 5 个输出信号供 View 订阅：

```cpp
signals:
    void tableStateReady(const TableViewState& state);    // 球位置 + 瞄准状态
    void cueStateReady(const CueViewState& state);        // 角度/力度/加塞
    void scoreStateReady(const ScoreViewState& state);    // 分数 + 犯规消息
    void gameInfoStateReady(const GameInfoViewState& state); // 阶段文本
    void shotAnimationCancelled();                        // 紧急中止动画
```

接收 View 的 5 个输入槽：

```cpp
public slots:
    void setAngle(double angle);          // 瞄准角度
    void setPower(double power);          // 击球力度
    void onShotAnimationFinished();       // 动画结束 → 触发物理
    void placeWhiteBall(double x, double y); // D 区放置坐标
    void restart();                       // 全盘重置
```

### 状态变换

`pushTableState()` 是最核心的变换函数：

```cpp
void GameSessionViewModel::pushTableState() {
    TableViewState state;

    // 1. 数据映射：Model 指针列表 → DTO 向量
    for (const auto& ball : m_gameState->balls()) {
        BallViewState bvs;
        bvs.x = ball->position().x;
        bvs.y = ball->position().y;
        bvs.type = static_cast<int>(ball->type());
        bvs.pocketed = ball->isPocketed();
        bvs.onTable = ball->isOnTable();
        state.balls.append(bvs);
    }

    // 2. 派生状态计算
    state.canAim = !isPlacingWhiteBall && !isSimulating
                && phase != NotStarted && phase != GameOver;
    state.canShoot = canAim && whiteBallOnTable;
    state.centeredCoordinates = /* 是否含有负坐标 */;

    emit tableStateReady(state);
}
```

View 不解析游戏规则——`canAim`、`canShoot`、`isSimulating`、`isPlacingWhiteBall` 等布尔字段直接控制交互状态。

### 模拟定时器驱动

```cpp
// ViewModel 持有 QTimer，由 Model 信号启停
connect(m_simulationTimer, &QTimer::timeout,
        this, &GameSessionViewModel::onSimulationTick);

void GameSessionViewModel::onModelSimulationStarted() {
    m_simulationTimer->start();  // performShot 后启动
}

void GameSessionViewModel::onSimulationTick() {
    m_gameState->updateSimulation();  // 一帧物理
    pushTableState();                  // 每帧刷新球位置
}

void GameSessionViewModel::onModelSimulationFinished() {
    m_simulationTimer->stop();  // 所有球静止后停止
    pushAllStates();
}
```

### 架构边界

- ViewModel 只 include Model 和 Common，不包含任何 Qt Widgets 头文件
- 不持有任何 View 对象的引用
- 所有跨层 `connect()` 在 `App.cpp` 中完成，ViewModel 和 View 互相不可见

### 绑定分类

| 类型 | 连接方 | 场所 | 数量 |
|------|--------|------|------|
| 通知绑定 | Model → ViewModel | ViewModel 构造函数 | 9 条 |
| 属性绑定 | ViewModel → View | App.cpp | 5 条 |
| 命令绑定 | View → ViewModel | App.cpp | 5 条 |
