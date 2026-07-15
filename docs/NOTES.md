# 开发日志

## 2026-07-11

- 拉取仓库，阅读 TASK_ALLOCATION.md，确认 Developer C 角色
- Common 层重构：整理 Constants.h/Types.h/MathUtils，统一代码风格
- 补充 Vector2D 复合赋值运算符和比较运算符
- 编译通过（20/20），推送到远端
- 环境适配：Qt 6.10.3（团队为 6.11.1），CMakePresets.json/vcpkg.json 做本机 skip-worktree

## 2026-07-12

- 阅读评分标准，明确中期小结评分要素
- 完成 MainWindow.cpp 两个 TODO：setupBindings() 和 initGame()
- 五层 MVVM 链路组装完成，编译通过（6/6）
- 窗口启动验证通过
- 创建 docs/ 文档结构：DESIGN.md / TEST.md / REPORTS / NOTES.md
- 准备中期报告材料

## 2026-07-16

- docs/ 目录整理：删除 DESIGN.md / MVVM_REFACTOR_GUIDE.md / app/* / common/design.md / B.md / TEST.md
- MVVM_FINAL_ARCHITECTURE.md → ARCHITECTURE.md，内部标题同步
- MODEL_VIEWMODEL.md 修正行数
- NOTES.md 补充近期记录

## 2026-07-14

- 中期报告最终定稿（合并为单份 .tex，8 页 PDF），含全部截图
- 推送中期报告至远端

## 2026-07-13

- B 推送物理引擎（球-球碰撞、球-库边碰撞、摩擦力），编译通过（22/22）
- B 改动 Common 层：Constants.h 物理参数调优、MathUtils 新增 closestPointOnSegment
- B 在 MainWindow 补充角度/力度实时同步连接（供瞄准线）
- **A/B 联合推送大型更新**（12 文件、730 行）：
  - GameView 鼠标交互：瞄准/击球改为鼠标操作，含球杆动画
  - 白球 D 区放置交互
  - GameInfoPanel 重构为 setViewModel 绑定
  - 新增比赛重启功能（View + Model/ViewModel）
  - Constants.h 新增 D 区常量
- 更新 docs/common/ 和 docs/app/ 文档以反映以上变更

## 接口变更记录

| 日期 | 变更 | 影响 |
|------|------|------|
| 07-11 | Common 层重构，常量/类型/工具函数定版 | Model/ViewModel 无须改动 |
| 07-12 | MainWindow.h 新增 m_gameState/m_cueViewModel/m_scoreViewModel 成员 | 仅 App 层内部 |
| 07-12 | B 改 Constants.h（摩擦0.99/弹性0.88/库边0.77），新增 closestPointOnSegment | Physics 层使用 |
| 07-12 | B 在 MainWindow 加角度/力度实时同步 | 瞄准线可用 |
| 07-13 | 击球流程：按钮→GameView 鼠标+动画，角度/力度双向同步 | CueControl 击球按钮废弃 |
| 07-13 | GameInfoPanel 改为 setViewModel 绑定 | MainWindow 减少 4 条转发连接 |
| 07-13 | 新增 GameVM::gameRestarted 信号，重启清空提示 | ScoreVM 状态重置 |
| 07-13 | Constants.h 新增 BAULK_LINE_X / D_RADIUS | D 区放置功能 |
| 07-13 | 新增 test_mathutils target + 60 条单元测试 | 验证 Common 层无回归 |
| 07-13 | 文档分 4 批提交（common/app/架构/报告） | C 的提交数 3→8 |
| 07-13 | 中期报告更新（含测试内容），.gitignore 加 LaTeX 产物 | 报告反映最新进度 |
| 07-13 | **MVVM 严格重构**：新增 contracts 层、GameUiBus、GameSessionViewModel | View 不再依赖 ViewModel/Model（编译期强制） |
| 07-13 | View 层 bind(GameUiBus*) 替代 setViewModel，删除全部 ViewModel include | 架构护栏 7/7 通过 |
| 07-13 | MainWindow 瘦身：删除 setupBindings()、9 条 connect，仅剩 Bus 注入 | App 层无业务逻辑 |
| 07-13 | 代码风格统一：去除 ASCII 分隔线注释（CMakeLists + 6 源文件） | 净减 78 行 |
| 07-13 | 更新 docs/ 全部架构文档，反映 contracts 层和新的依赖边界 | 文档与代码一致 |
| 07-14 | 中期报告定稿，合并为单份 .tex（8 页 PDF） | 提交 380362d |
| 07-15 | 最终架构文档与源码逐项核对，修正 4 处差异（信号/行数） | MVVM_FINAL_ARCHITECTURE.md |
| 07-16 | docs/ 目录清理：删 7 份过时/冗余文档，重命名架构文档 | 16→7 份 |
| 07-16 | 补充 NOTES.md 近期记录 | 本文 |
