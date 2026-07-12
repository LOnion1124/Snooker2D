# 测试文档

## 1. 测试策略

| 层级 | 测试类型 | 工具 | 负责人 |
|------|----------|------|--------|
| Common | 单元测试 | 手动验证 / 将来 CTest | C |
| Model | 单元测试 | 手动验证 / 将来 CTest | B |
| ViewModel | 集成测试 | 手动运行 + 日志 | B |
| View | UI 测试 | 人工交互 | A |
| App + 全链路 | 端到端 | 人工完整对局 | C |

## 2. Common 层测试用例

### MathUtils

| 用例 | 输入 | 期望输出 | 状态 |
|------|------|----------|:--:|
| dot-正交 | (1,0)·(0,1) | 0 | — |
| dot-同向 | (2,0)·(3,0) | 6 | — |
| cross | (1,0)×(0,1) | 1 | — |
| length | (3,4) | 5 | — |
| normalize-零向量 | (0,0) | (0,0) | — |
| normalize-单位向量 | (5,0) | (1,0) | — |
| reflect-垂直入射 | (1,-1), (0,1) | (1,1) | — |
| distance | (0,0)→(3,4) | 5 | — |
| circleOverlap-重叠 | 圆心距2, r1=2, r2=1 | true | — |
| circleOverlap-分离 | 圆心距10, r1=2, r2=1 | false | — |

### Types

| 用例 | 输入 | 期望输出 | 状态 |
|------|------|----------|:--:|
| ballValue-Red | Red | 1 | — |
| ballValue-Black | Black | 7 | — |
| ballValue-White | White | 0 | — |
| isColorBall-Red | Red | false | — |
| isColorBall-Blue | Blue | true | — |
| oppositePlayer-P1 | Player1 | Player2 | — |
| oppositePlayer-P2 | Player2 | Player1 | — |

## 3. App 层集成测试

| 用例 | 步骤 | 期望结果 | 状态 |
|------|------|----------|:--:|
| 窗口启动 | 运行 exe | 显示主窗口，五区域可见 | — |
| 控件注入 | 启动后检查 | GameView/CueControl/ScoreBoard 非空 | — |
| Model 初始化 | 启动后检查 | 22 颗球初始位置正确 | — |

## 4. Bug 记录

| 编号 | 发现日期 | 描述 | 严重程度 | 状态 |
|------|----------|------|----------|:--:|
| — | — | — | — | — |
