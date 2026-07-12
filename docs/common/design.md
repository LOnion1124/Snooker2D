# Common 层设计

提供所有层共用的基础类型、常量和数学工具函数。纯 C++，不依赖 Qt。

## 文件

| 文件 | 内容 |
|------|------|
| `Constants.h` | 球桌、球、袋口、物理、游戏参数常量 |
| `Types.h` | Vector2D、BallType、FoulType、GamePhase、PlayerId、辅助函数 |
| `MathUtils.h/.cpp` | 向量运算、几何检测 |

## 设计决策

### Vector2D

- 成员函数（`+`/`-`/`*`/`/`/`dot`/`normalized` 等）委托给 MathUtils 自由函数
- 补充 `+=`/`-=`/`*=`/`/=` 复合赋值和 `==`/`!=` 比较，满足物理引擎就地运算需求
- 一元 `-` 提供取反语义

### FoulResult 位置

`FoulResult` 结构体定义在 Model 层 `Rules.h`。原因：其成员 `QString description` 依赖 Qt，不适合放入纯 C++ 的 Common 层。ViewModel 通过前置声明 `struct FoulResult` 引用，编译无问题。

### 袋口坐标

袋口坐标（`Pocket` 结构体）定义在 Model 层 `Table.h`，由 `Table` 类统一管理球桌几何。`Constants.h` 仅存放 `POCKET_RADIUS` 单个标量。

### MathUtils 范围

已实现：`dot`、`cross`、`length`、`lengthSquared`、`normalize`、`reflect`、`distance`、`circleOverlap`、`closestPointOnSegment`。

- `closestPointOnSegment`：计算点到线段最近点，Physics 层球-库边碰撞检测调用（B 于 7/12 新增）

### 物理常量（7/12 B 调参）

- `FRICTION_COEFFICIENT` 0.98 → 0.99
- `COLLISION_RESTITUTION` 0.96 → 0.88
- 新增 `CUSHION_RESTITUTION` 0.77（库边弹性不同于球-球）

### D 区常量（7/13 A/B 新增）

白球落袋后需放置在 D 区内。新增两个常量：
- `BAULK_LINE_X = -TABLE_WIDTH * 0.3`（开球线 X 坐标，中心坐标系）
- `D_RADIUS = TABLE_HEIGHT * 0.35 / 2.0`（D 区半圆半径）

### ballValue / isColorBall / oppositePlayer

作为 `inline constexpr` 辅助函数放在 Types.h，供 Model/ViewModel 层查询球分值和玩家切换。`inline constexpr` 确保零运行时开销。
