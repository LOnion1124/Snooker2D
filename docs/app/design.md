# App 层设计

## 职责

应用程序入口、五层对象创建与组装、跨层信号/槽绑定、构建配置。

## 文件

| 文件 | 内容 |
|------|------|
| `main.cpp` | QApplication 创建，MainWindow 显示 |
| `AppConfig.h` | 窗口标题、版本、默认尺寸 |
| `MainWindow.h/.cpp` | 依赖注入、信号绑定、启动流程 |
| `CMakeLists.txt` | 五层 target 自底向上链接 |
| `vcpkg.json` | 依赖声明（当前为空） |
| `.gitignore` | 构建/IDE/Qt 生成文件 |

## MainWindow 设计

### 创建顺序

```
setupUI() → initGame() → setupBindings()
```

`initGame()` 必须在 `setupBindings()` 之前，因为信号连接依赖已创建的对象。

### initGame() 流程

```
new GameState → new GameViewModel.setGameState()
             → new CueControlViewModel
             → new ScoreViewModel
                  ↓ setViewModel()
             GameView / CueControl / ScoreBoard / GameInfoPanel
                  ↓
             GameState::startNewGame()
```

GameInfoPanel 于 7/13 改为 `setViewModel(GameViewModel*)`，内部自行绑定 currentPlayer/gamePhase 属性，不再由 MainWindow 逐条转发。

### setupBindings() 信号连接

共 9 条跨层连接：

| 信号 | 方向 | 目标 |
|------|------|------|
| GameView::shotAnimationFinished | → | GameVM::shoot() |
| CueControlVM::angleChanged | → | GameVM::setAngle |
| CueControlVM::powerChanged | → | GameVM::setPower |
| GameVM::cueAngleChanged | → | CueControlVM::setAngle（反向同步） |
| GameVM::cuePowerChanged | → | CueControlVM::setPower（反向同步） |
| GameVM::playerScoreChanged | → | ScoreVM 同步 |
| GameVM::gameRestarted | → | ScoreVM 清空提示 |
| GameVM::foulOccurred | → | ScoreVM |
| GameVM::gameOver | → | ScoreVM |

**7/13 重构要点**：
- 击球触发从 CueControl 按钮改为 GameView 鼠标点击（含球杆动画）
- 角度/力度改为双向同步：鼠标在球桌上拖动可调角度/力度，控件滑块亦可
- GameInfoPanel 改为通过 setViewModel 自行绑定，不再逐信号转发
- 新增 gameRestarted 信号处理，重启时清空犯规/状态提示

## 构建设计

- Qt 通过系统预编译安装（`C:\Qt\6.10.3\mingw_64`），CMake `find_package` 解析
- 不通过 vcpkg 安装 Qt（MinGW triplet 会触发源码编译）
- 五层各为独立 static library，便于增量编译和单元测试
