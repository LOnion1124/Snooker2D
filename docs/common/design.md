# Common 层设计

## 结构

Common 层分为两个子层：

| 子层 | 文件 | 依赖 |
|------|------|------|
| 核心（snooker_common） | Constants.h / Types.h / MathUtils.h/.cpp | 无（纯 C++） |
| 契约（snooker_contracts） | contracts/GameViewState.h / GameUiBus.h / ContractsInit | Common + Qt::Core |

## 核心子层

### Constants.h

球桌 800×400、球半径 10、袋口半径 16、红球 15 颗、总计 22 颗。
物理参数：步长 1/60s、摩擦 0.99、球碰撞恢复 0.88、库边恢复 0.77、速度阈值 0.05。
D 区：BAULK_LINE_X、D_RADIUS。

### Types.h

Vector2D 结构体（14 个运算符，成员函数委托 MathUtils 自由函数）。
4 个枚举：BallType / FoulType / GamePhase / PlayerId。
3 个 inline constexpr 辅助函数：ballValue() / isColorBall() / oppositePlayer()。

### MathUtils.h/.cpp

9 个函数：dot / cross / length / lengthSquared / normalize / reflect / distance / circleOverlap / closestPointOnSegment。

设计原则：只实现当前被调用的函数，不做预铺。

## 契约子层（7/13 新增）

### GameViewState.h

5 个纯数据 DTO（BallViewState / TableViewState / CueViewState / ScoreViewState / GameInfoViewState）。
View 通过 DTO 接收状态快照，不直接访问 ViewModel getter。

关键字段：
- TableViewState::canAim / canShoot — View 据此控制交互，替代字符串解析
- GameInfoViewState::phaseKind — GamePhase 枚举值，供 View 做样式分支

### GameUiBus.h

QObject，12 条信号（5 条 ViewModel→View + 7 条 View→ViewModel）。不含业务逻辑，仅作为跨层通信契约。

### ContractsInit.h/.cpp

注册自定义 DTO 到 Qt 元对象系统，应用启动时调用一次。

## 设计决策

- FoulResult 在 Model 层 Rules.h（含 Qt 类型，不适合 Common）
- 袋口坐标在 Model 层 Table.h（几何数据归 Model 管理）
- contracts 独立编译为 snooker_contracts，保持 snooker_common 的纯 C++ 属性
