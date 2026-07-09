# 小组合作开发任务分配清单

> **项目**：Snooker2D — 基于 C++/Qt 6 的 2D 斯诺克小游戏  
> **架构**：MVVM（Model / View / ViewModel / Common / App 五层）  
> **团队**：3 名开发者 | **工期**：10 天  
> **范围**：基本功能（经典斯诺克规则、本地双人、分数计算、碰撞模拟、力度/角度调控）

---

## 1. 人员分工总览

| 开发者 | 负责层 | 核心职责 |
|--------|--------|----------|
| **开发者 A** | `src/view/` | Qt Widgets 界面实现：球桌渲染、球绘制、力度/角度控件、计分板、信息面板、主窗口布局 |
| **开发者 B** | `src/model/` + `src/viewmodel/` | 游戏状态与规则引擎（Model）；MVVM 桥接层，将 Model 数据转换为 View 可消费的属性和命令（ViewModel） |
| **开发者 C** | `src/common/` + `src/app/` | 公共类型/常量/工具函数（Common）；应用入口、主窗口组装、CMake/vcpkg 构建配置（App）；兼 **集成测试与 CI 维护** |

> **辅助分工**：开发者 A 兼 UI 资源管理（`assets/` 图片/音频）；开发者 B 兼物理与规则单元测试；开发者 C 兼项目构建配置与 `.gitignore` 维护。

---

## 2. 各层文件规划

### 2.1 Common 层（开发者 C）

| 文件 | 说明 |
|------|------|
| `src/common/Constants.h` | 球桌尺寸、球半径、袋口位置/大小、摩擦力系数、速度阈值等物理与游戏常量 |
| `src/common/Types.h` | 枚举/结构体：`BallType`（红/黄/绿/棕/蓝/粉/黑/白）、`FoulType`、`GamePhase`、`Player`、`Vector2D` |
| `src/common/MathUtils.h` / `.cpp` | 二维向量运算（加减、点积、叉积、归一化、反射）、线段与圆求交等 |

### 2.2 App 层（开发者 C）

| 文件 | 说明 |
|------|------|
| `src/main.cpp` | 应用程序入口，创建 `QApplication` 与主窗口 |
| `src/app/MainWindow.h` / `.cpp` | 主窗口：组装 View 层各控件，绑定 ViewModel |
| `src/app/AppConfig.h` | 应用级配置（窗口标题、默认尺寸等） |
| `CMakeLists.txt` | 顶层 CMake 构建脚本，分五层 target 链接 |
| `vcpkg.json` | vcpkg 清单，声明 Qt 6 依赖（`qtbase`, `qtmultimedia`） |
| `.gitignore` | C++/CMake/vcpkg 忽略规则 |

### 2.3 Model 层（开发者 B）

| 文件 | 说明 |
|------|------|
| `src/model/Ball.h` / `.cpp` | 球的属性：类型、位置、速度、半径、是否在台面/已落袋、颜色 |
| `src/model/Table.h` / `.cpp` | 球桌：尺寸、库边（6条）、6个袋口定义 |
| `src/model/Physics.h` / `.cpp` | 物理引擎：匀速直线运动、球-球弹性碰撞、球-库边反弹、摩擦力减速、袋口检测 |
| `src/model/GameState.h` / `.cpp` | 核心状态机：开球、交替击球、连击判断、犯规检测（空杆/打错球/白球落袋/球未碰库）、自由球、分数累计、输赢判定 |
| `src/model/Player.h` / `.cpp` | 玩家：姓名、当前得分 |
| `src/model/Rules.h` / `.cpp` | 斯诺克规则模块：合法击球判定、犯规类型枚举、罚分计算、复位白球逻辑 |

### 2.4 ViewModel 层（开发者 B）

| 文件 | 说明 |
|------|------|
| `src/viewmodel/GameViewModel.h` / `.cpp` | 核心 ViewModel：暴露 `Q_PROPERTY`（球位置列表、当前玩家、比分、阶段），提供 `Q_INVOKABLE` 方法（击球、切换角度/力度、确认犯规） |
| `src/viewmodel/CueControlViewModel.h` / `.cpp` | 击球控制：角度（0-360°）、力度（0-100%），击球点偏移（加塞预留） |
| `src/viewmodel/ScoreViewModel.h` / `.cpp` | 计分板数据：双方累计分、当前击球得分、犯规提示字符串 |

### 2.5 View 层（开发者 A）

| 文件 | 说明 |
|------|------|
| `src/view/GameView.h` / `.cpp` | `QWidget` 子类：`paintEvent` 绘制球桌（绿色台面、棕色库边、6袋口）、所有球、辅助瞄准线 |
| `src/view/CueControl.h` / `.cpp` | 力度/角度控件：滑块（`QSlider`）+ 数值显示，方向指示器 |
| `src/view/ScoreBoard.h` / `.cpp` | 计分面板：双方得分、当前击球信息、犯规提示 |
| `src/view/GameInfoPanel.h` / `.cpp` | 状态信息栏：当前玩家指示、阶段提示（"请击红球"/"请击彩球"/"犯规-自由球"等） |
| `src/view/MainWindow.h` / `.cpp` | 主窗口布局（水平/垂直排布上述控件） |

---

## 3. 10 天任务分解

### 第 1 天：项目骨架搭建 & 公共类型定义

| 开发者 | 任务 | 预计产出 |
|--------|------|----------|
| **C** | ① 初始化 CMakeLists.txt（5 层 target）与 vcpkg.json ② 编写 `Constants.h`、`Types.h` ③ 搭建 `main.cpp` 骨架（空窗口可运行） | 项目可构建、窗口可启动 |
| **B** | ① 定义 Model 层核心数据结构：`Ball`、`Table`、`Player`（头文件 + 桩实现）② 定义 ViewModel 层类骨架（头文件） | Model/ViewModel 类型声明就绪 |
| **A** | ① 搭建 MainWindow 布局框架（空占位）② 创建 `GameView` 空白画布类、`ScoreBoard`/`CueControl` 占位 Widget | 窗口骨架可展示 |

> **当日 Git 提交示例**：  
> `feat(common): add Constants.h and Types.h`  
> `feat(app): add CMakeLists.txt, vcpkg.json, main.cpp skeleton`  
> `feat(model): add Ball, Table, Player header stubs`  
> `feat(view): add MainWindow layout placeholder`

---

### 第 2 天：公共工具 & Model 基础 & View 初版渲染

| 开发者 | 任务 | 预计产出 |
|--------|------|----------|
| **C** | ① 完善 `MathUtils.h/.cpp`（向量运算、碰撞几何）② 完善 `AppConfig.h` ③ 搭建信号/槽总线（Common 层事件定义） | 工具函数可被 B 调用 |
| **B** | ① 完善 `Ball`/`Table` 实现（含初始化 22 颗球的标准斯诺克摆球）② 开始 `Physics` 匀速运动与摩擦力减速 | 球可直线运动并减速 |
| **A** | ① `GameView` 实现基本绘制：绿色台面 + 棕色库边 + 6 袋口 + 22 颗球（用 Model 桩数据验证渲染）② 准备球颜色资源 | 球桌画面可见 |

> **当日 Git 提交示例**：  
> `feat(common): implement MathUtils vector operations`  
> `feat(model): implement Ball placement and Table initialization`  
> `feat(view): render table surface, pockets, and balls`

---

### 第 3 天：物理引擎 & 击球控制界面

| 开发者 | 任务 | 预计产出 |
|--------|------|----------|
| **C** | ① 协助 B 联调 Common↔Model 接口 ② 完善 `.gitignore` ③ 开始 `MainWindow` 信号/槽组装 | 公共层与模型层接口对齐 |
| **B** | ① 球-球弹性碰撞 ② 球-库边反弹 ③ 袋口检测（球心与袋口距离 < 阈值） ④ `Physics` 模块单元测试 | 核心物理可运行 |
| **A** | ① `CueControl` 控件：力度滑块 + 角度旋钮/滑块 + 数值标签 ② 击球按钮（触发 ViewModel 命令） | 力度/角度可交互 |

> **当日 Git 提交示例**：  
> `feat(common): add event signal bus definitions`  
> `feat(model): implement ball-ball and ball-cushion collision`  
> `feat(model): add pocket detection`  
> `feat(view): implement CueControl power/angle sliders`

---

### 第 4 天：ViewModel 初步桥接 & 数据流动

| 开发者 | 任务 | 预计产出 |
|--------|------|----------|
| **C** | ① 整合 `MainWindow`：实例化 Model、ViewModel、View，完成依赖注入 ② 编写端到端冒烟测试（构建通过、窗口显示） | Model→ViewModel→View 链路可跑通 |
| **B** | ① `GameViewModel` 核心实现：暴露球位置列表（`Q_PROPERTY`）、击球命令 ② `CueControlViewModel` 绑定角度/力度属性 | ViewModel 可供 View 绑定 |
| **A** | ① `GameView` 绑定 `GameViewModel` 的球位置属性实现动态刷新 ② `CueControl` 绑定 `CueControlViewModel` | 界面随数据变化实时刷新 |

> **当日 Git 提交示例**：  
> `feat(viewmodel): implement GameViewModel ball position binding`  
> `feat(viewmodel): implement CueControlViewModel properties`  
> `feat(view): bind GameView to GameViewModel`  
> `feat(app): wire Model-ViewModel-View in MainWindow`

---

### 第 5 天：游戏规则引擎（核心）

| 开发者 | 任务 | 预计产出 |
|--------|------|----------|
| **C** | ① 重构 `MainWindow` 支持游戏启动/重置流程 ② 编写规则相关 Common 事件（犯规信号、回合切换信号） | 应用生命周期管理完善 |
| **B** | ① `GameState` 状态机：开球→交替击球→回合结束→犯规→自由球→比赛结束 ② `Rules` 模块：合法击球判定（当前应击红/彩）、犯规检测、罚分计算 ③ 白球落袋后复位（D 区放置） | 完整斯诺克规则可运行 |
| **A** | ① `GameInfoPanel` 实现：显示当前阶段（"红球"/"彩球"/"犯规"）、当前玩家 ② `ScoreBoard` 绑定 `ScoreViewModel` 实时显示比分 | 游戏状态信息可视化 |

> **当日 Git 提交示例**：  
> `feat(model): implement GameState state machine`  
> `feat(model): implement Rules foul detection and penalty calculation`  
> `feat(view): add GameInfoPanel with phase display`  
> `feat(view): bind ScoreBoard to ScoreViewModel`

---

### 第 6 天：完整游戏流程 & 击球操作链

| 开发者 | 任务 | 预计产出 |
|--------|------|----------|
| **C** | ① 端到端调试击球流程（选角度→选力度→击球→物理模拟→碰撞→回合判定）② 修复集成问题 | 完整一轮击球可走通 |
| **B** | ① `ScoreViewModel`：分数累计、犯规提示字符串 ② 白球复位交互（D 区鼠标选位）的 ViewModel 支持 ③ `GameState` 完善：连击（break）记录 | 计分逻辑完整 |
| **A** | ① 瞄准辅助线绘制（从白球出发沿角度方向虚线）② 白球 D 区放置交互 ③ 球落袋动画/移除效果 | 用户体验完善 |

> **当日 Git 提交示例**：  
> `feat(viewmodel): implement ScoreViewModel with break tracking`  
> `feat(model): add cue-ball reposition in D-zone`  
> `feat(view): draw aiming guide line`  
> `feat(view): cue-ball D-zone placement interaction`

---

### 第 7 天：犯规、自由球 & 边界情况

| 开发者 | 任务 | 预计产出 |
|--------|------|----------|
| **C** | ① 编写 Common 层单元测试（`MathUtils`、`Constants` 校验）② 维护 `DEVELOPMENT.md` 开发日志 | 公共代码有测试覆盖 |
| **B** | ① 完善所有犯规场景：空杆、先击中错误球、无球碰库、白球落袋、球飞出台面 ② 自由球（Free Ball）规则实现 ③ 所有犯规对应的罚分与回合转移 | 犯规系统完整 |
| **A** | ① 犯规弹窗/提示动画 ② 自由球选择交互（可选任意彩球替代红球）③ 复位白球 UI 完善 | 犯规与自由球 UI 就绪 |

> **当日 Git 提交示例**：  
> `feat(model): implement all foul scenarios`  
> `feat(model): add free-ball rule`  
> `feat(view): add foul notification dialog`  
> `feat(view): free-ball selection UI`

---

### 第 8 天：输赢判定 & 比赛结束

| 开发者 | 任务 | 预计产出 |
|--------|------|----------|
| **C** | ① 比赛结束/重启流程的 App 层支持 ② 协助 A 做整体 UI 走查 | 比赛生命周期完善 |
| **B** | ① 输赢判定逻辑（台面红球清空 + 彩球按序清空 + 分差不可追）② `GameState` 结束状态与胜者输出 ③ Model 层完整单元测试 | 比赛可正常结束并判定胜负 |
| **A** | ① 比赛结束画面/弹窗 ② 重新开始按钮 ③ 整体 UI 美化（字体、颜色、间距） | 完整游戏流程可从头到尾走通 |

> **当日 Git 提交示例**：  
> `feat(model): implement win/loss determination`  
> `feat(view): add game-over dialog with winner display`  
> `feat(app): support game restart lifecycle`

---

### 第 9 天：集成测试 & Bug 修复

| 开发者 | 任务 | 预计产出 |
|--------|------|----------|
| **C** | ① 全程集成测试（至少 5 局完整比赛）② 修复 App/Common 层 Bug ③ 确保 CMake 构建 Release/Debug 均通过 | 构建稳定、零构建错误 |
| **B** | ① 修复 Model/ViewModel 层 Bug ② 补充边界测试（极端角度、零力度、所有球贴库等）③ 性能检查（物理循环是否流畅） | 物理与规则零逻辑缺陷 |
| **A** | ① 修复 View 层 Bug（绘制闪烁、布局错位等）② 窗口缩放适配 ③ 打击感优化（击球音效预留接口、力度反馈） | UI 流畅、缩放正常 |

> **当日 Git 提交示例**：  
> `fix(model): handle edge case when all reds pocketed`  
> `fix(view): fix flickering on rapid repaint`  
> `fix(app): correct signal-slot disconnection on restart`

---

### 第 10 天：最终交付 & 文档

| 开发者 | 任务 | 预计产出 |
|--------|------|----------|
| **C** | ① 填写 `README.md`（项目介绍、构建说明、运行方式、操作指南）② 清理调试代码 ③ 最终 Release 构建验证 | README 完善、项目可交付 |
| **B** | ① 代码注释与 Doxygen 风格文档字符串 ② 补充 `DEVELOPMENT.md` 中 Model/ViewModel 架构说明 | 代码可维护 |
| **A** | ① `assets/` 资源最终整理 ② 截图/演示准备 ③ 最终 UI 走查 | 资源就绪、演示可用 |
| **全员** | ① 代码 Review 交叉检查 ② 最终合并与 Git 标签 `v1.0.0` | 项目交付 |

> **当日 Git 提交示例**：  
> `docs: complete README.md with build and play instructions`  
> `chore: remove debug logs and cleanup`  
> `docs(model): add Doxygen comments for physics engine`  
> `chore(assets): finalize game sprites and sounds`

---

## 4. Git 提交规范

为在 Git 历史中清晰体现 MVVM 架构分工，所有 commit message 统一采用以下格式：

```
<type>(<layer>): <简短描述>
```

| type | 含义 |
|------|------|
| `feat` | 新功能 |
| `fix` | Bug 修复 |
| `refactor` | 重构 |
| `docs` | 文档 |
| `test` | 测试 |
| `chore` | 杂项（构建、资源、清理） |

| `<layer>` （必填） | 对应目录 |
|--------------------|----------|
| `common` | `src/common/` |
| `app` | `src/app/` + `src/main.cpp` + `CMakeLists.txt` |
| `model` | `src/model/` |
| `viewmodel` | `src/viewmodel/` |
| `view` | `src/view/` |
| `assets` | `assets/` |
| `docs` | `docs/` |

**示例**：
```
feat(model): implement ball-ball elastic collision
feat(viewmodel): expose ball positions as Q_PROPERTY
feat(view): bind GameView repaint to GameViewModel signals
feat(app): wire Model-ViewModel-View dependency injection
fix(common): correct Vector2D reflection formula
docs: add task allocation and architecture overview
```

> 每条 commit 只涉及**单一 layer**，便于通过 `git log --oneline --grep="(model)"` 等命令按层过滤历史。

---

## 5. 接口约定（跨层协作关键）

为避免集成时大量返工，三层之间需提前约定以下接口：

### 5.1 Common ↔ Model（C 与 B）

- `Vector2D` 结构体定义在 `src/common/Types.h`，Model 层直接使用
- 物理常量（摩擦系数、碰撞恢复系数等）定义在 `src/common/Constants.h`
- Model 层不依赖 Qt（纯 C++），便于单元测试

### 5.2 Model ↔ ViewModel（B 内部）

- Model 层所有类继承 `QObject`，使用 Qt 信号/槽通知状态变更
- ViewModel 通过 `Q_PROPERTY` 暴露数据，通过 `Q_INVOKABLE` 暴露命令
- 数据流方向：`Model (signal)` → `ViewModel (property update)` → `View (binding)`

### 5.3 ViewModel ↔ View（A 与 B）

| ViewModel 属性/命令 | 类型 | View 绑定 |
|---------------------|------|-----------|
| `ballPositions` | `QVariantList` | `GameView` 重绘球位置 |
| `currentPlayer` | `int` | `GameInfoPanel` 显示 |
| `gamePhase` | `QString` | `GameInfoPanel` 阶段提示 |
| `player1Score` / `player2Score` | `int` | `ScoreBoard` 数字 |
| `cueAngle` | `double` | `CueControl` 角度控件 |
| `cuePower` | `double` | `CueControl` 力度滑块 |
| `shoot()` | `Q_INVOKABLE` | "击球"按钮点击 |
| `confirmFoul()` | `Q_INVOKABLE` | 犯规弹窗确认 |

### 5.4 View ↔ App（A 与 C）

- View 层所有控件为独立可复用 `QWidget` 子类
- `MainWindow`（App 层）负责创建所有 View/ViewModel/Model 实例并完成依赖注入
- 控件之间不直接通信，通过 ViewModel 中转

---

## 6. 风险与应对

| 风险 | 概率 | 应对 |
|------|------|------|
| 物理引擎碰撞检测有 Bug（球穿透、抖动） | 中 | 第 3 天 B 优先完成单元测试；引入固定时间步长（`deltaTime` 固定值） |
| ViewModel 属性绑定不生效 | 中 | 第 4 天 C 协助 A+B 做端到端验证；Qt 属性绑定用 `Q_PROPERTY` + `NOTIFY` 信号 |
| 斯诺克规则遗漏边界情况 | 高 | B 在第 5 天参考世界斯诺克协会（WPBSA）官方规则文档逐条对照 |
| 集成时接口不匹配 | 中 | 严格遵循第 5 节接口约定；每天合并前互相通知接口变更 |
| 第 9 天 Bug 积压过多 | 低 | 每日构建、每日集成；第 8 天起冻结新功能 |

---

## 7. 每日站会议程（建议）

每天 10 分钟，每人回答三个问题：
1. 昨天完成了什么？（对照本清单当日任务）
2. 今天计划做什么？有无阻塞？
3. 接口有无变更需要同步？（更新第 5 节约定）

---

> **文档版本**：v1.0  
> **制定日期**：2026-07-09  
> **下次评审**：第 5 天中期检查点
