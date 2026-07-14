# App 层设计

## 职责

应用程序入口、View 和 ViewModel 的对象创建与 Bus 绑定。**App 层不承载游戏业务逻辑。**

## 文件

| 文件 | 内容 |
|------|------|
| `main.cpp` | QApplication 创建，MainWindow 显示 |
| `AppConfig.h` | 窗口标题、版本、默认尺寸 |
| `MainWindow.h/.cpp` | 创建 Bus、绑定 View、启动 |
| `CMakeLists.txt` | 六层 target 自底向上链接，架构护栏检查 |
| `vcpkg.json` | 依赖声明（当前为空） |
| `.gitignore` | 构建/IDE/Qt/LaTeX 生成文件 |

## MainWindow 设计

### 创建顺序

```
setupUI() → initGame()
```

`setupBindings()` 已删除（7/13 重构）。跨层协调全部交给 GameSessionViewModel。

### initGame() 流程

```
new GameSessionViewModel      // 内部创建 Bus + Model + 子 VM
m_uiBus = vm->bus()           // 获取 Bus
view->bind(m_uiBus)           // 4 个 View 统一绑定
vm->start()                   // 推送初始状态 + 开始游戏
```

### 与旧版对比

| 维度 | 重构前 | 重构后 |
|------|--------|--------|
| 成员变量 | 8 个（1 Model + 3 VM + 4 View） | 6 个（1 Bus + 1 VM + 4 View） |
| 方法 | setupUI + initGame + setupBindings | setupUI + initGame |
| 信号连接 | 9 条 connect | 0 条 |
| 业务逻辑 | 比分、角度同步、犯规提示 | 全部下沉到 GameSessionViewModel |

## 构建设计

- Qt 通过系统预编译安装，CMake `find_package` 解析
- `snooker_view` 不链接 `snooker_viewmodel` / `snooker_model`（编译期强制）
- 架构护栏：`cmake --build build --target check_architecture`
- 五层 + contracts 各为独立 static library
