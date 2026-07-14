# App 层测试

## 集成测试

### 窗口启动

| 步骤 | 期望 |
|------|------|
| 运行 Snooker2D.exe | 窗口标题 "Snooker2D"，尺寸约 1280×720 |
| 检查布局 | 左侧球桌+击球控制，右侧计分板+信息面板 |
| 检查控件 | 4 个 View 控件均非空 |

### Bus 注入

| 检查项 | 方法 |
|--------|------|
| GameView 持有 Bus | `m_gameView->m_bus != nullptr` |
| CueControl 持有 Bus | `m_cueControl->m_bus != nullptr` |
| ScoreBoard 持有 Bus | `m_scoreBoard->m_bus != nullptr` |
| GameInfoPanel 持有 Bus | `m_gameInfoPanel->m_bus != nullptr` |
| 所有 View 绑定同一 Bus | 四个 m_bus 指针值相等 |

### 信号通路

| 场景 | 预期行为 |
|------|----------|
| 鼠标点击球桌 | GameView 播放杆动画 → emit shotAnimationFinished → Bus 转发 → SessionVM::onShotAnimationFinished |
| 鼠标移动 | GameView 计算角度 → emit bus->cueAngleRequested → SessionVM 同步到子 VM |
| 滚轮调力度 | GameView → emit bus->cuePowerRequested → SessionVM 同步到子 VM |
| 切换玩家 | SessionVM 推送 gameInfoStateChanged → GameInfoPanel 更新 |
| 犯规发生 | SessionVM 推送 scoreStateChanged → ScoreBoard 显示犯规信息 |
| 重启按钮 | GameInfoPanel → emit bus->restartRequested → SessionVM::onRestartRequested |

## 架构护栏

| 检查项 | 命令 |
|--------|------|
| View 不含 viewmodel/model include | `cmake --build build --target check_architecture` |
| View 不含 setViewModel | 同上 |
| snooker_view 不链接 viewmodel/model | 同上 |

## 端到端测试

| 场景 | 步骤 | 期望 |
|------|------|------|
| 完整一局 | 启动→交替击球→有人获胜 | 无崩溃，流程完整 |
| 重启 | 点击重启按钮 | 状态重置，22 球归位 |
| D 区摆球 | 白球落袋后 | 可在 D 区内点击放置白球 |
