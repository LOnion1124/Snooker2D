# Common 层测试

## 向量运算

| 函数 | 输入 | 期望 | 结果 |
|------|------|------|:--:|
| dot | (1,0)·(0,1) | 0 | |
| dot | (2,0)·(3,0) | 6 | |
| cross | (1,0)×(0,1) | 1 | |
| length | (3,4) | 5 | |
| normalize | (5,0) | (1,0) | |
| normalize | (0,0) | (0,0) | |
| reflect | (1,-1) 法线(0,1) | (1,1) | |
| circleOverlap | 圆心距2, r1=2, r2=1 | true | |
| circleOverlap | 圆心距10, r1=2, r2=1 | false | |
| distance | (0,0)→(3,4) | 5 | |
| closestPointOnSegment | P(0,0) 段(2,0)-(2,4) | (2,0) | |
| closestPointOnSegment | P(0,2) 段(2,0)-(2,4) | (2,2) | |
| closestPointOnSegment | P(3,2) 段(0,0)-(2,0) | (2,0) | |

## 类型辅助函数

| 函数 | 输入 | 期望 | 结果 |
|------|------|------|:--:|
| ballValue | Red | 1 | |
| ballValue | Black | 7 | |
| ballValue | White | 0 | |
| isColorBall | Red | false | |
| isColorBall | Blue | true | |
| oppositePlayer | Player1 | Player2 | |
| oppositePlayer | Player2 | Player1 | |

## Vector2D 运算符

| 运算符 | 输入 | 期望 | 结果 |
|--------|------|------|:--:|
| += | (1,2)+=(3,4) | (4,6) | |
| -= | (5,3)-=(2,1) | (3,2) | |
| *= | (2,3)*=2 | (4,6) | |
| /= | (6,8)/=2 | (3,4) | |
| == | (1,2)==(1,2) | true | |
| != | (1,2)!=(3,4) | true | |
| - (一元) | -(3,-4) | (-3,4) | |
