# Developer B 个人反思与收获

> **角色**：开发者 B — 负责 Model 层 + ViewModel 层  
> **时间**：2026-07-09 至 2026-07-14

## 一、工作概览

### Model 层

| 任务 | 说明 |
|------|------|
| initBalls | 标准斯诺克 22 球摆球：白球 D 区中央、6 颗彩球各自点位、15 颗红球三角阵（正六边形密排） |
| 物理引擎 | 匀速运动、球-球弹性碰撞（0.88）、球-库边碰撞（0.77）、摩擦衰减（0.99）、袋口检测 |
| 边界约束 | 两层方案：硬约束（台面 +5px 阈值）+ 慢球回拉（速度 < 0.1 时拉回台面） |
| D 区放置 | 开球线 + 半圆几何判定 + 球间重叠检测 |
| 状态机 | performShot → updateSimulation → handlePostShot 完整击球流程 |
| 彩球复位 | 红球未清空前彩球进球后 `respot()` 回到初始点位 |
| 犯规判罚 | 白球落袋/空杆/先击中错误球/无球碰库四类犯规检测与罚分计算 |
| 比赛结束 | 全台清空后判定 GameOver，比分离者胜出 |

### ViewModel 层

| 任务 | 说明 |
|------|------|
| GameSessionViewModel | 从零搭建，经历三次架构迭代（子 VM → 合并 → 取消 Bus） |
| 信号重构 | 从 GameUiBus 12 条信号改为自管信号/槽，App 层集中绑定 |
| 状态推送 | pushTableState/pushCueState/pushScoreState/pushGameInfoState 四路 DTO 推送 |
| 模拟循环 | QTimer 驱动 ~60FPS 物理循环，onSimulationTick 每帧推球位置 |
| Bug 修复 | 碰撞方向反转、瞄准线坐标偏移、滑块不联动、击球动画状态恢复等 |

## 二、最大挑战：物理调试

物理引擎是这次开发中最难的部分。核心困难不在公式本身，而在**多球密集接触时的稳定性**。

### 问题

两个看似正确的东西放在一起产生了错误：弹性碰撞公式单独测试 OK，边界约束单独测试 OK，但红球三角阵中多球同时碰撞时，球被链式推出边界。

### 解决方案：多层防御

```
move → ball-ball collision → cushion collision → pocket → friction
  → hardConstraint（台面 +5px 截断）→ pullBack（慢球回拉）
```

- **第一层**：碰撞响应本身（位置分离 + 弹性速度交换）
- **第二层**：硬约束强制截断（`applyHardConstraint`），处理链式推出
- **第三层**：慢球回拉（`checkPullBackBalls`），处理硬约束也未覆盖的边界情况

### 教训

物理模拟不是"找到正确公式就完事"的工程。每个子系统的误差会在后续步骤放大，多层防御比单层精确公式更可靠。`step()` 的调用顺序同样关键——移动必须在碰撞之前，摩擦必须在最后。

此外，一个方向判断错误花了很长时间排查：

```cpp
// 错误：if (velAlongNormal > 0) return;  // 以为 >0 是"已分离"
// 正确：if (velAlongNormal <= 0) return; // <=0 才是"正在分离"
```

`velAlongNormal > 0` 意味着距离在减少（正在接近），但直觉上容易误读为"正速度 = 正在远离"。写物理代码必须画图确认方向约定。

## 三、架构演进的三次重构

### 第一轮：子 ViewModel 拆分

最初设计了三个子 ViewModel（GameViewModel / CueControlViewModel / ScoreViewModel），各自暴露 `Q_PROPERTY`。问题：层数过多，每帧数据要经过 Model → GameVM → CueVM/ScoreVM → SessionVM → Bus 四层转发，连接关系复杂。

### 第二轮：合并为单一 ViewModel

将三个子 VM 合并到 GameSessionViewModel，去掉中间转发层。数据流简化为 Model → SessionVM → Bus → View。但 Bus 本身仍然是冗余抽象。

### 第三轮：取消 Bus

Bus 的最大问题是"修改信号就要改 Common"，而 Common 是所有人都依赖的底层，任何改动都会触发全量编译。改为各层自管信号/槽后，Common 只保留 DTO，不再包含通信基础设施。

### 教训

中间抽象层不应被设计为"万能通道"。Bus 在刚设计时很精简，但随着功能增加不断膨胀，最终成为修改瓶颈。各层只暴露最小必要接口，连接关系集中在 App 层，反而是最干净的方案。

## 四、协作模式

与 Developer A（View）的协作完全通过 DTO 接口契约完成：

1. 我在 `GameViewState.h` 中定义 `BallViewState`、`TableViewState` 等 DTO
2. 确保 DTO 包含 View 需要的所有派生状态（`canAim`、`canShoot`、`centeredCoordinates`）
3. Developer A 在 `GameView::applyTableState()` 中消费 DTO，独立完成绘制
4. 双方不需要了解对方类的内部实现

当 View 需要新的交互（如白球放置）：

1. View 新增信号 `whiteBallPlacementRequested(double x, double y)`
2. ViewModel 新增槽 `placeWhiteBall(double x, double y)`
3. App 层新增一条 `connect`

三个文件各改一处，互不侵入。这种模式让并行开发基本没有阻塞。

## 五、收获总结

- **Debug 顺序决定效率**：先检查数据源（Model 输出的坐标），再检查变换层（ViewModel 的 DTO 构造），最后检查展示（View 绘制）。跳过顺序会反复在错误层找原因。
- **信号连接时机很重要**：Qt 6 中 `connect(nullptr, ...)` 会 crash。必须先 new 对象再 connect，不能先 connect 再构造。
- **最小依赖原则的实践**：Common 层保持纯 C++ 不依赖 Qt，使 Model 的单元测试可以脱离 Qt 框架运行；ViewModel 不依赖 Qt Widgets，确保不会意外引用 UI 类型。
- **重构的前置条件是测试**：三次架构迭代中，每次都有之前的编译期护栏（CMake 依赖 + 架构检查脚本）兜底，否则手动验证无法覆盖所有连接路径。
