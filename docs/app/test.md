# App 层测试

## 集成测试

### 窗口启动

| 步骤 | 期望 |
|------|------|
| 运行 Snooker2D.exe | 窗口标题 "Snooker2D"，尺寸约 1280×720 |
| 检查布局 | 左侧球桌+击球控制，右侧计分板+信息面板 |
| 检查控件 | GameView/CueControl/ScoreBoard/GameInfoPanel 均非空 |

### 依赖注入

| 检查项 | 方法 |
|--------|------|
| GameView 持有 ViewModel | `m_gameView->m_viewModel != nullptr` |
| CueControl 持有 ViewModel | `m_cueControl->m_viewModel != nullptr` |
| ScoreBoard 持有 ViewModel | `m_scoreBoard->m_viewModel != nullptr` |
| GameInfoPanel 持有 ViewModel | `m_gameInfoPanel->m_viewModel != nullptr` |

### 信号通路

| 场景 | 预期行为 |
|------|----------|
| 鼠标点击球桌 | GameView 播放杆动画 → shotAnimationFinished → GameVM::shoot() |
| 拖动角度滑块 | GameVM::cueAngle 跟随变化，CueControlVM 同步 |
| 拖动力度滑块 | GameVM::cuePower 跟随变化，CueControlVM 同步 |
| 鼠标在球桌上调角度 | CueControlVM::angle 跟随变化（反向同步） |
| 切换玩家 | GameInfoPanel 更新当前玩家显示 |
| 阶段变化 | GameInfoPanel 更新阶段文本 |
| 犯规发生 | ScoreBoard 显示犯规信息 |
| 重启比赛 | ScoreBoard 犯规/状态信息清空 |

## 端到端测试

| 场景 | 步骤 | 期望 |
|------|------|------|
| 完整一局 | 启动→交替击球→有人获胜 | 无崩溃，流程完整 |
| 重启 | 比赛结束→新游戏 | 状态重置，22 球归位 |
| D 区摆球 | 白球落袋后 | 可在 D 区内点击放置白球 |
