# 加塞功能开发文档

本文档定义加塞（English/Sidespin）功能的接口契约和分工，A/B/C 三方可按各自部分并行开发。

## 功能描述

玩家击球时选择击打白球的不同位置，施加旋转（加塞），影响白球走位：

- **上塞（Top/Follow）**：击打白球上部，碰撞后白球继续向前滚动
- **下塞（Bottom/Draw）**：击打白球下部，碰撞后白球回拉
- **左塞/右塞（Left/Right）**：击打白球左右侧，影响库边反弹角度

## 物理模型变更

当前物理模型是**无旋质点碰撞**——球只有位置和线速度，碰撞响应仅处理法线方向的弹性交换。

加塞要求升级为**有旋刚体碰撞**——引入角速度（spin），碰撞时在切线方向通过摩擦力耦合线速度和角速度：

```
接触点相对速度 = (v1 - v2)·tangent + (ω1 + ω2) × radius

碰撞前：切线方向有滑动 → 摩擦力冲量同时改变线速度和角速度
碰撞后：球获得/失去旋转，白球走位改变
```

这需要三层改动：Common 层提供切线工具和常量，Ball 类增加角速度状态，Physics 重写碰撞响应的切线部分。

## 数据表示

加塞用二维向量 `(englishX, englishY)` 表示，范围 **[-1.0, 1.0]**：

```
englishX > 0  → 右塞（从球员视角）
englishX < 0  → 左塞
englishY > 0  → 上塞（Follow）
englishY < 0  → 下塞（Draw）
(0, 0)        → 正中击球（无旋转）
```

该字段**已经存在**于 `CueViewState`（`src/common/contracts/GameViewState.h:33-34`）和 `GameSessionViewModel`（`src/viewmodel/GameSessionViewModel.h:60-61`），当前值恒为 0。

角速度 `ω`（angularVelocity）用 `double` 表示，单位 rad/s，正值 = 顺时针（从上方看）。这是新增的 Ball 成员。

---

## 分工总览

| 层 | 负责人 | 工作内容 |
|----|--------|----------|
| Common | C | MathUtils 新增 `tangent()`；Constants 新增加塞物理常量 |
| Model | B | Ball 新增 `angularVelocity`；Physics 重写碰撞响应（切线冲量）和摩擦（自旋衰减+滚动耦合）；GameState 扩展 `performShot` |
| View | A | CueControl 加塞 UI + GameView 白球击打点标记 |
| ViewModel | B | 新增 `setEnglish` 槽，击球时传入 Model |
| App | C | 新增 1 条 connect |

**并行策略**：

```
第 1 步（C，先提交）：MathUtils + Constants
    ↓
第 2 步（A 和 B 并行）：A 做 UI，B 做物理 + ViewModel
    ↓
第 3 步（C，最后对接）：App.cpp 加 connect
```

---

## 第一部分：Common 层（开发者 C）

### 1.1 MathUtils — 新增切线函数

文件：`src/common/MathUtils.h` / `.cpp`

旋转物理中频繁需要从法线向量求切线向量。新增：

```cpp
// 返回 v 逆时针旋转 90° 的向量
// tangent(Vector2D(1,0)) = Vector2D(0,1)
// tangent(Vector2D(0,1)) = Vector2D(-1,0)
Vector2D tangent(const Vector2D& v);
```

实现：

```cpp
Vector2D MathUtils::tangent(const Vector2D& v) {
    return Vector2D(-v.y, v.x);
}
```

对应的顺时针切线为 `-tangent(normal)`，不需要单独函数。

### 1.2 Constants — 新增加塞物理常量

文件：`src/common/Constants.h`

```cpp
// ---- 加塞物理 ----

// 击球时 english 值到白球初始角速度的转换
// 击球后白球 ω = englishY * ENGLISH_TO_SPIN（上塞正，下塞负）
// 同时左右塞影响出杆方向有微小偏移（实际由 Physics 处理）
constexpr double ENGLISH_TO_SPIN = 50.0;       // english ±1 → ±50 rad/s

// 球-球碰撞切线摩擦系数
// 控制碰撞时切线方向的摩擦力冲量大小
// 太小 → 加塞无效果；太大 → 碰撞后球飞出不合理方向
constexpr double BALL_BALL_TANGENT_FRICTION = 0.15;

// 球-库边碰撞切线摩擦系数
// 控制库边反弹时左/右塞对角度的影响
constexpr double CUSHION_TANGENT_FRICTION = 0.25;

// 自旋衰减（每物理帧，因子 * 60 换算到每秒）
// 球在台面上旋转因台泥摩擦力逐渐消失
constexpr double SPIN_DECAY = 0.98;           // 每帧保留 98%

// 滚动摩擦耦合系数
// 当球线速度方向和自旋方向不匹配时（滑动状态），逐步将其拉向纯滚动
// 纯滚动条件：v_tangential = ω × radius
constexpr double ROLLING_COUPLING = 0.02;     // 每帧耦合强度
```

### 1.3 单元测试

文件：`tests/test_mathutils.cpp`

新增 `tangent` 测试用例（3-5 条即可）：

```cpp
// tangent 基本方向
tangent(Vector2D(1,0)) → Vector2D(0,1)
tangent(Vector2D(0,1)) → Vector2D(-1,0)
tangent(Vector2D(-1,0)) → Vector2D(0,-1)
tangent(Vector2D(0,-1)) → Vector2D(1,0)
// tangent 的非零向量
tangent(Vector2D(3,4)) → Vector2D(-4,3)
```

---

## 第二部分：Model 层 — Ball（开发者 B）

### 2.1 新增角速度状态

文件：`src/model/Ball.h`

```cpp
// 属性访问（新增）
double angularVelocity() const { return m_angularVelocity; }

// 状态修改（新增）
void setAngularVelocity(double w);

private:
    double m_angularVelocity = 0.0;  // 新增：角速度 (rad/s)，正值=顺时针
```

文件：`src/model/Ball.cpp`

```cpp
void Ball::setAngularVelocity(double w) {
    m_angularVelocity = w;
}
```

角速度无需 Q_PROPERTY 绑定（View 不需要直接读角速度，走 DTO 即可），也不用 signal。

---

## 第三部分：Model 层 — 物理引擎（开发者 B）

这是改动最大的部分。当前碰撞响应只做**法线方向的弹性交换**（第 161-171 行），加塞需要增加**切线方向的摩擦冲量**。

### 3.1 核心物理公式

两个球碰撞时，在接触点处的相对速度包含线速度和角速度两部分：

```
normal  = (b.pos - a.pos).normalized()
tangent = MathUtils::tangent(normal)  // 逆时针 90°

// 接触点相对速度（法线分量 — 同旧版）
vn_rel = (va - vb) · normal

// 接触点相对速度（切线分量 — 新增，含角速度！）
vt_rel = (va - vb) · tangent + (ωa + ωb) × BALL_RADIUS
```

如果 `vt_rel ≠ 0`，说明两球在接触点有滑动摩擦。摩擦力冲量同时改变：
- 两球的线速度（切线方向）
- 两球的角速度

### 3.2 重写 resolveBallCollision

文件：`src/model/Physics.cpp` — `resolveBallCollision()`

在现有法线碰撞响应**之后**，增加切线摩擦处理：

```cpp
void Physics::resolveBallCollision(Ball& a, Ball& b, BallType* ioFirstHit) {
    Vector2D normal = b.position() - a.position();
    double dist = normal.length();
    double overlap = 2.0 * BALL_RADIUS - dist;

    if (dist < 1e-9) {
        normal = Vector2D(1.0, 0.0);
        overlap = 2.0 * BALL_RADIUS;
    } else {
        normal = normal.normalized();
    }

    // 1. 位置分离（不变）
    if (overlap > 0.0) {
        Vector2D correction = normal * (overlap * 0.5);
        a.setPosition(a.position() - correction);
        b.setPosition(b.position() + correction);
    }

    // 2. 法线方向速度响应（不变 — 等质量弹性碰撞）
    Vector2D relVel = a.velocity() - b.velocity();
    double velAlongNormal = relVel.dot(normal);
    if (velAlongNormal <= 0) return;

    double restitution = COLLISION_RESTITUTION;
    double impulseScalar = -(1.0 + restitution) * velAlongNormal * 0.5;
    Vector2D impulse = normal * impulseScalar;
    a.setVelocity(a.velocity() + impulse);
    b.setVelocity(b.velocity() - impulse);

    // 3. 切线方向摩擦响应（新增）
    // 接触点切线方向相对速度 = 线速度切线分量 + 角速度贡献
    Vector2D tangent = MathUtils::tangent(normal);
    double vtA = a.velocity().dot(tangent);
    double vtB = b.velocity().dot(tangent);
    double spinContribution = (a.angularVelocity() + b.angularVelocity()) * BALL_RADIUS;
    double vtRel = (vtA - vtB) + spinContribution;

    if (std::abs(vtRel) > 1e-6) {
        // 摩擦力冲量 = 切线相对速度 × 摩擦系数 × 质量（质量=1，简化）
        // 冲量方向与相对滑动方向相反
        double frictionImpulse = -vtRel * BALL_BALL_TANGENT_FRICTION;
        Vector2D tangentImpulse = tangent * frictionImpulse;

        a.setVelocity(a.velocity() + tangentImpulse);
        b.setVelocity(b.velocity() - tangentImpulse);

        // 角速度变化：摩擦力冲量产生的力矩
        // Δω = (tangentImpulse × radius) / I  其中 I = 0.4*m*r² (实心球)
        // 简化：Δω = frictionImpulse / (0.4 * BALL_RADIUS)
        double angularChange = frictionImpulse / (0.4 * BALL_RADIUS);
        a.setAngularVelocity(a.angularVelocity() - angularChange);
        b.setAngularVelocity(b.angularVelocity() - angularChange);
        // 注：a 和 b 的角速度变化方向相同（切线冲量造成同向旋转）
        // 因为冲量作用在接触点，对 a 产生的力矩方向与对 b 相反
        // 简化模型：切线相对速度减少 → 两球都获得同向旋转
    }

    // 4. 跟踪白球首次击中（不变）
    if (ioFirstHit && *ioFirstHit == BallType::White) {
        if (a.type() == BallType::White && b.type() != BallType::White) {
            *ioFirstHit = b.type();
        } else if (b.type() == BallType::White && a.type() != BallType::White) {
            *ioFirstHit = a.type();
        }
    }
}
```

### 3.3 重写 resolveCushionCollision

文件：`src/model/Physics.cpp` — `resolveCushionCollision()`

库边碰撞同样需要在切线方向处理旋转：

```cpp
void Physics::resolveCushionCollision(Ball& ball, const Vector2D& closestPoint,
                                      const Vector2D& inwardNormal) {
    // 1. 位置修正（不变）
    const double signedDistance = MathUtils::dot(ball.position() - closestPoint, inwardNormal);
    double overlap = BALL_RADIUS - signedDistance;
    if (overlap > 0.0) {
        ball.setPosition(ball.position() + inwardNormal * overlap);
    }

    // 2. 法线方向速度反射（不变）
    double velAlongNormal = MathUtils::dot(ball.velocity(), inwardNormal);
    if (velAlongNormal < 0.0) {
        Vector2D reflected = ball.velocity()
            - inwardNormal * ((1.0 + CUSHION_RESTITUTION) * velAlongNormal);
        ball.setVelocity(reflected);
    }

    // 3. 切线方向摩擦（新增）— 左/右塞影响反弹角度
    Vector2D tangent = MathUtils::tangent(inwardNormal);
    double vt = ball.velocity().dot(tangent);
    double spinVt = ball.angularVelocity() * BALL_RADIUS;
    double slipSpeed = vt - spinVt;  // 球在库边的滑动速度

    if (std::abs(slipSpeed) > 1e-6) {
        double frictionImpulse = -slipSpeed * CUSHION_TANGENT_FRICTION;
        ball.setVelocity(ball.velocity() + tangent * frictionImpulse);

        // 摩擦力矩改变角速度
        double angularChange = frictionImpulse / (0.4 * BALL_RADIUS);
        ball.setAngularVelocity(ball.angularVelocity() + angularChange);
    }
}
```

### 3.4 重写 applyFriction — 新增自旋衰减和滚动耦合

文件：`src/model/Physics.cpp` — `applyFriction()`

```cpp
void Physics::applyFriction(std::vector<Ball*>& balls, double deltaTime) {
    for (auto* ball : balls) {
        if (ball->isPocketed()) continue;

        // 3.1 线速度摩擦（不变）
        Vector2D vel = ball->velocity();
        double speed = vel.length();
        if (speed < MIN_VELOCITY) {
            ball->setVelocity(Vector2D(0.0, 0.0));
        } else {
            double friction = 1.0 - (1.0 - FRICTION_COEFFICIENT) * deltaTime * 60.0;
            ball->setVelocity(vel * friction);
        }

        // 3.2 角速度衰减（新增）
        double w = ball->angularVelocity();
        if (std::abs(w) > 0.01) {
            w *= SPIN_DECAY;
            ball->setAngularVelocity(w);
        } else {
            ball->setAngularVelocity(0.0);
        }

        // 3.3 滚动耦合（新增）
        // 当球在滑动（线速度与滚动不一致时），逐步拉向纯滚动
        // 纯滚动条件：v · tangent_方向 = ω × R
        // 简化：比较线速度方向与角速度对应的滚动方向
        if (speed > MIN_VELOCITY && std::abs(w) > 0.01) {
            Vector2D forward = vel.normalized();
            Vector2D rollTangent = MathUtils::tangent(forward);
            double rollSpeed = w * BALL_RADIUS;  // 角速度对应的线速度

            // 沿切线方向的耦合：角速度 → 线速度
            double coupling = rollSpeed * ROLLING_COUPLING;
            ball->setVelocity(vel + rollTangent * coupling);

            // 反向耦合：线速度 → 角速度
            double vt = vel.dot(rollTangent);
            double angularCoupling = (vt - rollSpeed) * ROLLING_COUPLING / BALL_RADIUS;
            ball->setAngularVelocity(w + angularCoupling * 0.5);
        }

        // 3.4 球完全停下时角速度也清零
        if (speed < MIN_VELOCITY && std::abs(ball->angularVelocity()) < 0.01) {
            ball->setAngularVelocity(0.0);
        }
    }
}
```

### 3.5 allBallsStopped — 考虑角速度

文件：`src/model/Physics.cpp` — `allBallsStopped()`

```cpp
bool Physics::allBallsStopped(const std::vector<Ball*>& balls) {
    for (const auto* ball : balls) {
        if (!ball->isPocketed()) {
            if (ball->velocity().length() > MIN_VELOCITY) return false;
            if (std::abs(ball->angularVelocity()) > 0.05) return false;  // 新增
        }
    }
    return true;
}
```

### 3.6 GameState::performShot — 传入加塞并设置初始自旋

文件：`src/model/GameState.h`

```cpp
// 签名修改
void performShot(double angle, double power,
                 double englishX = 0.0, double englishY = 0.0);
```

文件：`src/model/GameState.cpp`

```cpp
void GameState::performShot(double angle, double power,
                             double englishX, double englishY) {
    if (m_simulationRunning || m_phase == GamePhase::GameOver || m_whiteBallPlacing) return;

    Ball* whiteBall = /* ... 找到白球 ... */;
    if (!whiteBall) return;

    // 线速度（不变）
    double rad = angle * 3.14159265358979323846 / 180.0;
    double speed = (power / 100.0) * MAX_SPEED;
    whiteBall->setVelocity(Vector2D(std::cos(rad) * speed, -std::sin(rad) * speed));

    // 角速度（新增）— 由 english 值转换
    // englishY 控制上下旋转（主要分量），englishX 控制左右旋转（侧旋分量较小）
    double spin = englishY * ENGLISH_TO_SPIN;
    whiteBall->setAngularVelocity(spin);

    // englishX 影响出杆方向（微调角度，模拟击球点偏移）
    // 可选：不做方向微调，完全由切线摩擦在碰撞时体现

    // 快照（不变）
    // ...
}
```

### 3.7 Physics 的 english 成员

Physics 不需要单独存储 englishX/Y。english 在 `performShot` 中转换为白球的初始角速度后，后续物理模拟完全由角速度驱动。不需要在 Physics 中额外保存 english 值。

**但**如果后续想实现"击球点偏移影响出杆方向"（englishX 让白球初始方向微偏），可以在 Physics 或 GameState 中增加一个成员。由 B 自行决定是否实现。

---

## 第四部分：ViewModel 层（开发者 B）

### 4.1 新增 `setEnglish` 槽

文件：`src/viewmodel/GameSessionViewModel.h`

```cpp
public slots:
    void setEnglish(double englishX, double englishY);  // 新增
```

### 4.2 实现

文件：`src/viewmodel/GameSessionViewModel.cpp`

```cpp
void GameSessionViewModel::setEnglish(double englishX, double englishY) {
    auto clamp = [](double v) {
        if (v < -1.0) return -1.0;
        if (v > 1.0)  return 1.0;
        return v;
    };
    englishX = clamp(englishX);
    englishY = clamp(englishY);

    if (m_englishX != englishX || m_englishY != englishY) {
        m_englishX = englishX;
        m_englishY = englishY;
        pushCueState();
        pushTableState();   // 同时刷新 GameView 的击打点标记
    }
}
```

**m_englishX / m_englishY 已经存在**于 ViewModel 中（第 60-61 行），无需新增成员。

### 4.3 击球时传入 Model

修改 `onShotAnimationFinished()`：

```cpp
void GameSessionViewModel::onShotAnimationFinished() {
    if (m_gameState) {
        m_gameState->performShot(m_cueAngle, m_cuePower, m_englishX, m_englishY);
    }
}
```

### 4.4 重启时清零

在 `restart()` 中补上（当前遗漏了）：

```cpp
m_englishX = 0.0;
m_englishY = 0.0;
```

### 4.5 pushTableState 补充 english 字段

文件：`src/viewmodel/GameSessionViewModel.cpp` — `pushTableState()`

在现有 `state.cueAngle` / `state.cuePower` 赋值后追加：

```cpp
state.cueEnglishX = m_englishX;
state.cueEnglishY = m_englishY;
```

这需要 `TableViewState` 新增两个字段（见第六节 Common 层 DTO 变更）。

---

## 第五部分：View 层（开发者 A）

### 5.1 CueControl — 加塞控制面板

文件：`src/view/CueControl.h` / `.cpp`

**新增信号**：

```cpp
signals:
    void englishChanged(double englishX, double englishY);
```

**新增 UI**（在 `setupUI()` 中）：

建议方案：2×2 方向按钮 + 中心复位按钮，或一个可拖拽的 2D 网格（更直观）。

最小实现（按钮方案）：

```
          [↑ 上塞]
[← 左塞] [· 中心] [→ 右塞]
          [↓ 下塞]
```

- 四个方向按钮各设 englishX/Y = ±1.0
- 中心按钮复位为 (0,0)
- 每次点击 emit `englishChanged`
- 组合按钮（如同时按 ↑ 和 → ）→ (0.7, 0.7) 或允许逐次点击叠加

**applyCueState 同步**：

```cpp
void CueControl::applyCueState(const CueViewState& state) {
    // ... 已有逻辑 ...
    m_state = state;
    // 高亮当前加塞方向按钮
    refreshTexts();
}
```

**refreshTexts 更新**：显示当前加塞描述（如"加塞: 上右"）。

### 5.2 GameView — 白球击打点标记

文件：`src/view/GameView.h` / `.cpp`

在 `drawBalls()` 中，白球表面画击打点标记。需要一个来源——当前 TableViewState 不含 englishX/Y。两个方案：

**方案 A（推荐）**：在 `TableViewState` 中新增两个字段：

```cpp
// GameViewState.h — TableViewState
double cueEnglishX = 0.0;  // 新增
double cueEnglishY = 0.0;  // 新增
```

ViewModel 的 `pushTableState()` 中填充：

```cpp
state.cueEnglishX = m_englishX;
state.cueEnglishY = m_englishY;
```

**方案 B**：GameView 缓存 `m_cachedEnglishX/Y`，通过新的 slot `applyCueState` 接收（和其他 View 一样只收一个 DTO），但这样 GameView 需要额外的 DTO 传递。方案 A 更简单。

采用方案 A。GameView 从 TableViewState 中读取 `cueEnglishX/Y`，在白球表面画红点：

```cpp
// drawBalls 中，绘制白球后：
if (/* 当前球是白球 */ && !m_isShotAnimating) {
    double dotX = state.cueEnglishX * BALL_RADIUS * 0.6;  // 像素偏移
    double dotY = state.cueEnglishY * BALL_RADIUS * 0.6;
    // 在白球位置 + (dotX, -dotY) 处画红色小圆点（半径 2px）
}
```

---

## 第六部分：App 层（开发者 C）

### 6.1 新增连接

文件：`src/app/App.cpp`

在 "View → ViewModel（命令绑定）" 区域新增：

```cpp
QObject::connect(cueControl, &CueControl::englishChanged,
        &sessionViewModel, &GameSessionViewModel::setEnglish);
```

跨层 connect 从 10 条变为 11 条（5 属性 + 6 命令）。

---

## 接口契约（严格定义）

以下为本次功能涉及的全部接口变更。未列出的信号/槽/方法**不得修改签名**。

### 一、新增信号（View 层声明并发射）

| 信号 | 所属类 | 参数 | 触发时机 |
|------|--------|------|----------|
| `englishChanged(double englishX, double englishY)` | `CueControl` | englishX/Y: 范围 [-1.0, 1.0] | 用户操作加塞控件 |

### 二、新增槽（ViewModel 层声明）

| 槽 | 所属类 | 参数 | 行为 |
|----|--------|------|------|
| `setEnglish(double englishX, double englishY)` | `GameSessionViewModel` | englishX/Y: 范围 [-1.0, 1.0]，内部钳位 | 存储 → pushCueState() + pushTableState() |

### 三、现有槽的内部改动（签名不变）

| 槽 | 所属类 | 改动 |
|----|--------|------|
| `onShotAnimationFinished()` | `GameSessionViewModel` | 调用 `performShot(angle, power, englishX, englishY)` 替代原两参数版本 |
| `restart()` | `GameSessionViewModel` | 追加 `m_englishX = 0.0; m_englishY = 0.0;` |

以下槽**不需要任何改动**：
- `setAngle(double)` / `setPower(double)` / `placeWhiteBall(double,double)`（ViewModel）
- `applyTableState(const TableViewState&)` / `applyCueState(const CueViewState&)` / `applyScoreState(const ScoreViewState&)` / `applyGameInfoState(const GameInfoViewState&)` / `cancelShotAnimation()`（View 层全部）

### 四、现有信号不变

`GameSessionViewModel` 的 5 个信号（`tableStateReady` / `cueStateReady` / `scoreStateReady` / `gameInfoStateReady` / `shotAnimationCancelled`）**签名不变**。它们推送的 DTO 内容有变化（见第六节），但 connect 语句不受影响。

### 五、App 层新增 connect

文件：`src/app/App.cpp`

```cpp
// 在 "View → ViewModel（命令绑定）" 块末尾新增一行：
QObject::connect(cueControl, &CueControl::englishChanged,
        &sessionViewModel, &GameSessionViewModel::setEnglish);
```

现有 10 条 connect **全部保持不变**。加塞功能只增加 1 条，总计 11 条（5 属性 + 6 命令）。

### 六、DTO 变更（只加字段，不改已有）

| DTO | 新增字段 | 类型 | 默认值 | 填充位置 |
|-----|----------|------|--------|----------|
| `TableViewState` | `cueEnglishX` | `double` | `0.0` | `pushTableState()` |
| `TableViewState` | `cueEnglishY` | `double` | `0.0` | `pushTableState()` |

`CueViewState` 的 `englishX` / `englishY` **已存在**，无需修改。
`ScoreViewState` / `GameInfoViewState` 不加新字段。

### 七、Model 层接口变更

| 类 | 方法/成员 | 签名 | 说明 |
|----|-----------|------|------|
| `Ball` | `angularVelocity()` | `double angularVelocity() const` | **新增** getter |
| `Ball` | `setAngularVelocity(double)` | `void setAngularVelocity(double w)` | **新增** setter |
| `Ball` | `m_angularVelocity` | `double = 0.0` | **新增** 成员变量 |
| `GameState` | `performShot` | `void performShot(double angle, double power, double englishX = 0.0, double englishY = 0.0)` | **修改**：增加后两个默认参数 |
| `Physics` | `allBallsStopped` | 签名不变 | **内部逻辑**：增加角速度判断 |

### 八、Common 层新增

| 类型 | 名称 | 定义 |
|------|------|------|
| 函数 | `MathUtils::tangent` | `Vector2D tangent(const Vector2D& v)` → `Vector2D(-v.y, v.x)` |
| 常量 | `ENGLISH_TO_SPIN` | `50.0` |
| 常量 | `BALL_BALL_TANGENT_FRICTION` | `0.15` |
| 常量 | `CUSHION_TANGENT_FRICTION` | `0.25` |
| 常量 | `SPIN_DECAY` | `0.98` |
| 常量 | `ROLLING_COUPLING` | `0.02` |

### 九、完整信号链路

```
用户点击加塞按钮（CueControl）
  → emit englishChanged(englishX, englishY)
  → App.cpp connect
  → GameSessionViewModel::setEnglish(englishX, englishY)
    → m_englishX/Y 存储
    → pushCueState() → emit cueStateReady(CueViewState{englishX, englishY, ...})
      → App.cpp connect → CueControl::applyCueState() → 更新 UI 显示
    → pushTableState() → emit tableStateReady(TableViewState{cueEnglishX, cueEnglishY, ...})
      → App.cpp connect → GameView::applyTableState() → 缓存 + 绘制白球击打点

用户点击击球
  → emit shotAnimationFinished
  → App.cpp connect
  → GameSessionViewModel::onShotAnimationFinished()
    → GameState::performShot(angle, power, englishX, englishY)
      → whiteBall.setAngularVelocity(englishY * ENGLISH_TO_SPIN)
      → 物理模拟中 resolveBallCollision / resolveCushionCollision / applyFriction
         使用 Ball::angularVelocity() 参与切线摩擦计算
```

---

## 验证清单

- [ ] `test_mathutils` 新增 tangent 测试通过
- [ ] `cmake --build build` 编译通过
- [ ] `check_architecture` 护栏通过
- [ ] CueControl 加塞控件可见，操作后 emit `englishChanged`
- [ ] GameView 白球表面有击打点标记（红点）
- [ ] App.cpp 中 `englishChanged → setEnglish` 连接存在
- [ ] 上塞击球后白球碰撞目标球后有跟球效果
- [ ] 下塞击球后白球碰撞后有拉回效果
- [ ] 左/右塞击球后库边反弹角度有可见变化
- [ ] 球静止后角速度也归零
- [ ] 重启后加塞值归零（UI + ViewModel）
- [ ] 无加塞时物理效果与旧版一致（不退化）
