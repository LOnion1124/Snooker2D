# 项目架构文档

本文档记录 Snooker2D 项目经过三轮重构后的最终 MVVM 架构。

## 重构历程

| 阶段 | 内容 | 提交 |
|------|------|------|
| 第一轮 | 引入 contracts 层（GameUiBus）和 GameSessionViewModel，View 不再依赖 ViewModel/Model | `8a6bacc` |
| 第二轮 | 删除子 ViewModel（GameViewModel/CueControlViewModel/ScoreViewModel），全部合并到 GameSessionViewModel | `7fdad0d` |
| 第三轮 | 取消 GameUiBus，各层自管 signal/slot，App 层集中绑定 | `fd51e3e` |

## 最终架构

### 依赖图

```
Common 层（GameViewState DTO / Constants / Types / MathUtils）
    ↑               ↑                ↑
   View         ViewModel          Model
  (slots/       (slots/           (signals)
   signals)      signals)
    │               │                │
    └───────────────┼────────────────┘
                    │ 全部 connect 在 App.cpp
                 App 层
```

### 编译依赖（CMake 强制）

| Target | 链接 | 说明 |
|--------|------|------|
| `snooker_common` | 无 | 纯 C++ |
| `snooker_model` | common + Qt::Core | 不依赖 View/ViewModel |
| `snooker_viewmodel` | model + common + Qt::Core | 不依赖 View |
| `snooker_view` | common + Qt::Widgets | 不依赖 ViewModel/Model |
| `Snooker2D` | view + viewmodel + model + common + Qt::Widgets | 唯一汇聚点 |

### 层间通信

View、ViewModel、Model 各自声明自己的 signal/slot，互不 include 对方的头文件。App 层在 `App.cpp` 中完成全部 `connect()` 调用。

## 两种绑定（App 层集中）

App 层集中管理需要解耦的两组绑定。通知绑定（Model→ViewModel）留在 ViewModel 内部——因为 ViewModel 本就持有 Model 指针，两者是翻译关系而非绑定关系。

### 通知绑定：Model → ViewModel（ViewModel 内部）

Model 发射自身业务信号，ViewModel 构造时在内部 connect。

```cpp
// GameSessionViewModel 构造函数
connect(m_gameState, &GameState::phaseChanged,
        this, &GameSessionViewModel::onModelPhaseChanged);
connect(m_gameState, &GameState::simulationFinished,
        this, &GameSessionViewModel::onModelSimulationFinished);
// 共 9 条，全部在 ViewModel 内部
```

为什么不在 App 层：ViewModel 和 Model 之间是翻译关系——ViewModel 持有 Model 指针、调用 Model 方法。connect 放在构造函数更自然，不需要暴露 getter。

### 属性绑定：ViewModel → View

ViewModel 将 Model 数据转换为 ViewState DTO，通过自身信号推送。App 层 connect 到 View 的 public slots。

```cpp
// App.cpp
QObject::connect(&sessionViewModel, &GameSessionViewModel::tableStateReady,
        gameView, &GameView::applyTableState);
```

### 命令绑定：View → ViewModel

View 发射用户交互信号（`angleChanged`、`powerChanged` 等），App 层 connect 到 ViewModel 的 public slots。

```cpp
// App.cpp
QObject::connect(gameView, &GameView::angleChanged,
        &sessionViewModel, &GameSessionViewModel::setAngle);
```

## 关键代码

### App.cpp — 属性绑定和命令绑定集中于此

```cpp
int App::run(int argc, char* argv[]) {
    QApplication app(argc, argv);

    GameSessionViewModel sessionViewModel;
    MainWindow mainWindow;

    GameView* gameView = mainWindow.gameView();
    CueControl* cueControl = mainWindow.cueControl();
    ScoreBoard* scoreBoard = mainWindow.scoreBoard();
    GameInfoPanel* gameInfoPanel = mainWindow.gameInfoPanel();

    // ViewModel → View（属性绑定）
    QObject::connect(&sessionViewModel, &GameSessionViewModel::tableStateReady,
            gameView, &GameView::applyTableState);
    QObject::connect(&sessionViewModel, &GameSessionViewModel::cueStateReady,
            cueControl, &CueControl::applyCueState);
    QObject::connect(&sessionViewModel, &GameSessionViewModel::scoreStateReady,
            scoreBoard, &ScoreBoard::applyScoreState);
    QObject::connect(&sessionViewModel, &GameSessionViewModel::gameInfoStateReady,
            gameInfoPanel, &GameInfoPanel::applyGameInfoState);
    QObject::connect(&sessionViewModel, &GameSessionViewModel::shotAnimationCancelled,
            gameView, &GameView::cancelShotAnimation);
    // 共 5 条

    // View → ViewModel（命令绑定）
    QObject::connect(gameView, &GameView::angleChanged,
            &sessionViewModel, &GameSessionViewModel::setAngle);
    QObject::connect(gameView, &GameView::powerChanged,
            &sessionViewModel, &GameSessionViewModel::setPower);
    QObject::connect(gameView, &GameView::shotAnimationFinished,
            &sessionViewModel, &GameSessionViewModel::onShotAnimationFinished);
    QObject::connect(gameView, &GameView::whiteBallPlacementRequested,
            &sessionViewModel, &GameSessionViewModel::placeWhiteBall);
    QObject::connect(gameInfoPanel, &GameInfoPanel::restartRequested,
            &sessionViewModel, &GameSessionViewModel::restart);
    // 共 5 条

    sessionViewModel.start();
    mainWindow.show();
    return app.exec();
}
```

### GameSessionViewModel.h — 自声明信号和槽

```cpp
class GameSessionViewModel : public QObject {
    Q_OBJECT
public:
    void start();

public slots:
    // View 命令
    void setAngle(double angle);
    void setPower(double power);
    void onShotAnimationFinished();
    void placeWhiteBall(double x, double y);
    void restart();

signals:
    void tableStateReady(const TableViewState& state);
    void cueStateReady(const CueViewState& state);
    void scoreStateReady(const ScoreViewState& state);
    void gameInfoStateReady(const GameInfoViewState& state);
    void shotAnimationCancelled();

private slots:
    // Model 信号回调（构造函数中 connect）
    void onModelPhaseChanged(GamePhase phase);
    void onModelSimulationStarted();
    void onModelSimulationFinished();
    // ...
};
```

### GameSessionViewModel 构造函数 — 通知绑定在内部

```cpp
GameSessionViewModel::GameSessionViewModel(QObject* parent) : QObject(parent) {
    m_gameState = new GameState(this);
    // ...
    // Model 信号 → 内部处理（通知绑定，ViewModel 和 Model 本就耦合）
    connect(m_gameState, &GameState::phaseChanged,
            this, &GameSessionViewModel::onModelPhaseChanged);
    connect(m_gameState, &GameState::simulationFinished,
            this, &GameSessionViewModel::onModelSimulationFinished);
    // 共 9 条
}
```

### GameView.h — 自声明信号和槽

```cpp
class GameView : public QWidget {
    Q_OBJECT
public slots:
    void applyTableState(const TableViewState& state);
    void cancelShotAnimation();

signals:
    void angleChanged(double angle);
    void powerChanged(double power);
    void whiteBallPlacementRequested(double x, double y);
    void shotAnimationFinished();
};
```

### CueControl.h — 纯接收

```cpp
class CueControl : public QWidget {
    Q_OBJECT
public slots:
    void applyCueState(const CueViewState& state);
};
```

### ScoreBoard.h — 纯接收

```cpp
class ScoreBoard : public QWidget {
    Q_OBJECT
public slots:
    void applyScoreState(const ScoreViewState& state);
};
```

### GameInfoPanel.h — 接收 + 发射

```cpp
class GameInfoPanel : public QWidget {
    Q_OBJECT
public slots:
    void applyGameInfoState(const GameInfoViewState& state);
signals:
    void restartRequested();
};
```

### MainWindow.h — 暴露子 View getter

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    GameView*      gameView()      const;
    CueControl*    cueControl()    const;
    ScoreBoard*    scoreBoard()    const;
    GameInfoPanel* gameInfoPanel() const;
private:
    void setupUI();
    GameView*      m_gameView;
    CueControl*    m_cueControl;
    ScoreBoard*    m_scoreBoard;
    GameInfoPanel* m_gameInfoPanel;
};
```

### View 发射信号的方式

```cpp
// GameView::mouseMoveEvent → updateCueAngleFromMouse
void GameView::updateCueAngleFromMouse(const QPointF& mousePosition) {
    double angle = std::atan2(-dy, dx) * 180.0 / π;
    emit angleChanged(angle);    // 不经过任何中间件，直接发射自身信号
}

// GameView::wheelEvent
void GameView::wheelEvent(QWheelEvent* event) {
    const double newPower = m_cachedCuePower - wheelSteps * 5.0;
    emit powerChanged(newPower);
}

// GameInfoPanel — 重启按钮直接发射信号
connect(m_restartButton, &QPushButton::clicked,
        this, &GameInfoPanel::restartRequested);
```

### ViewModel 状态推送

```cpp
void GameSessionViewModel::pushTableState() {
    TableViewState state;
    // 从 GameState 读取球位置，填充 DTO
    for (const auto& ball : m_gameState->balls()) {
        BallViewState bvs;
        bvs.x = ball->position().x;
        bvs.y = ball->position().y;
        bvs.type = static_cast<int>(ball->type());
        state.balls.append(bvs);
    }
    // 交互状态判定
    state.canAim = !isWhiteBallPlacing && !isSimulating
                && phase != GamePhase::NotStarted;
    // 推送到 View
    emit tableStateReady(state);
}
```

## 关键设计点

### DTO 在 Common 层

`TableViewState`、`CueViewState`、`ScoreViewState`、`GameInfoViewState` 定义在 `src/common/contracts/GameViewState.h`。这是唯一被 View 和 ViewModel 共享的类型——因为数据格式本身不属于任何一层。

### View 层不解析游戏规则

View 的所有交互判断通过 ViewState DTO 中的布尔字段（`canAim`、`canShoot`、`isSimulating`）驱动，不通过中文字符串或枚举值做业务分支。`phaseKind` 仅用于样式选择。

### 通知绑定在 ViewModel 内部

Model→ViewModel 的 connect 留在 GameSessionViewModel 构造函数。因为 ViewModel 本就持有 Model 指针并直接调用其方法——两者是翻译关系，不是绑定关系。把这种 connect 放到 App 层只会增加不必要的 getter 暴露。

### 架构护栏

`tests/architecture/check_mvvm_boundaries.ps1` 在每次构建时检查：
- View 不 include ViewModel/Model 头文件
- View 不引用具体 ViewModel 类名
- `snooker_view` 不链接 `snooker_viewmodel`/`snooker_model`

## 对比：重构前后

| 维度 | 重构前 | 重构后 |
|------|--------|--------|
| View 耦合 | include 具体 ViewModel 类 | 只 include Common（GameViewState.h） |
| App 层职责 | 创建对象 + 9 条 connect 业务协调 | 创建对象 + 10 条 connect（属性+命令绑定） |
| 通信中间件 | GameUiBus（12 信号） | 无（直接 signal↔slot） |
| 通知绑定 | SessionVM 内部 connect | 同左（ViewModel 内部，不改动） |
| 属性/命令绑定 | 分散在 SessionVM + 4 个 View | 全部在 App.cpp |
| 子 ViewModel | GameVM/CueVM/ScoreVM 三个 | 全部合并到 GameSessionViewModel |
| CMake target | 6 个 | 4 个 |

## 文件清单

| 层 | 文件 | 行数 |
|----|------|:--:|
| Common | `Constants.h` `Types.h` `MathUtils.h/.cpp` `contracts/GameViewState.h` | — |
| Model | `Ball` `Table` `Physics` `GameState` `Player` `Rules` | — |
| ViewModel | `GameSessionViewModel.h/.cpp` | 317 |
| View | `GameView` `CueControl` `ScoreBoard` `GameInfoPanel` `MainWindow` | — |
| App | `App.h/.cpp` `main.cpp` | 74 |
