# 开发环境搭建指南

> **项目**: 2D 台球小游戏 (Billiard)  
> **适用平台**: Windows 10/11 + VS Code  
> **最后更新**: 2026-07-08

---

## 一、环境概览

| 组件 | 版本 | 用途 |
|------|------|------|
| 操作系统 | Windows 11 (x64) | — |
| 编译器 | **MinGW-w64 (g++ 14.2.0)** | C++ 编译 |
| 构建系统 | **CMake 4.1.2** | 项目构建配置 |
| 构建工具 | **Ninja 1.13.2** | 快速增量编译 |
| GUI 框架 | **Qt 6.11.1** | 桌面界面 |
| 包管理器 | **vcpkg** (2026-05-27) | 第三方库管理 |
| 编辑器 | **VS Code** | 代码编辑与调试 |

---

## 二、安装步骤

### 2.1 安装 MinGW-w64

**方式一：MSYS2 (推荐)**

1. 下载 [MSYS2](https://www.msys2.org/) 并安装
2. 打开 **MSYS2 UCRT64** 终端，执行：
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-gdb mingw-w64-ucrt-x86_64-cmake
   ```
3. 将 `C:\msys64\ucrt64\bin` 添加到系统 `PATH`

**方式二：WinGet**

```powershell
winget install -e --id GnuWin32.Gcc
```

**方式三：MinGW-Builds 独立包**

1. 访问 https://github.com/niXman/mingw-builds-binaries/releases
2. 下载 `x86_64-14.2.0-release-win32-seh-ucrt-rt_v12-rev0.7z`
3. 解压到 `C:\Program Files\mingw64`
4. 将 `C:\Program Files\mingw64\bin` 添加到系统 `PATH`

> ⚠️ 本项目使用 UCRT 运行时 (非 MSVCRT)，请选择 `ucrt` 变体。

### 2.2 安装 CMake

**WinGet (推荐):**

```powershell
winget install -e --id Kitware.CMake
```

或从 https://cmake.org/download/ 下载 Windows x64 Installer。

安装后验证：

```powershell
cmake --version
# 预期输出: cmake version 4.1.2
```

### 2.3 安装 Ninja

**WinGet (推荐):**

```powershell
winget install -e --id Ninja-build.Ninja
```

或从 https://github.com/nickstenning/ninja/releases 下载 `ninja-win.zip`，将 `ninja.exe` 放到 `PATH` 中的目录。

安装后验证：

```powershell
ninja --version
# 预期输出: 1.13.2
```

### 2.4 安装 Qt 6

1. 访问 https://www.qt.io/download-qt-installer
2. 下载 **Qt Online Installer** 并运行
3. 在 `Select Components` 页面，选择：
   - **Qt 6.11.1** → `MSVC 2022 64-bit` 或 `MinGW 64-bit`
   - **Qt 6.11.1** → 展开 `Additional Libraries`，勾选：
     - `Qt Multimedia`（音频播放）
4. 默认安装路径：`C:\Qt\6.11.1\mingw_64`

> 📌 本项目示例环境使用 MinGW 64-bit 变体，安装路径为 `C:\Qt\6.11.1\mingw_64`。

安装后验证：

```powershell
qmake6 --version
# 预期输出: Using Qt version 6.11.1 in C:/Qt/6.11.1/mingw_64/lib
```

### 2.5 安装 vcpkg

```powershell
# 1. 克隆仓库
git clone https://github.com/microsoft/vcpkg.git D:\SomeApps\vcpkg

# 2. 运行 bootstrap
D:\SomeApps\vcpkg\bootstrap-vcpkg.bat

# 3. 设置环境变量 (建议添加到系统环境变量)
setx VCPKG_ROOT "D:\SomeApps\vcpkg"
```

> 📌 vcpkg 安装路径可自由选择，本项目使用 `D:\SomeApps\vcpkg`。

安装后验证：

```powershell
vcpkg --version
# 预期输出: vcpkg package management program version 2026-05-27-...
```

### 2.6 配置环境变量

将以下路径添加到 **系统 PATH**（按实际安装路径调整）：

```
C:\Program Files\mingw64\bin
C:\Qt\6.11.1\mingw_64\bin
C:\Program Files\CMake\bin
D:\SomeApps\vcpkg
```

**系统环境变量：**

| 变量名 | 值 |
|--------|-----|
| `VCPKG_ROOT` | `D:\SomeApps\vcpkg` |
| `CMAKE_PREFIX_PATH` | `C:\Qt\6.11.1\mingw_64` (可选) |

---

## 三、VS Code 配置

### 3.1 推荐安装的扩展

在 VS Code 中按 `Ctrl+Shift+X`，搜索并安装：

| 扩展 ID | 名称 | 用途 |
|----------|------|------|
| `ms-vscode.cpptools` | C/C++ | C++ 语法高亮、智能提示、调试 |
| `ms-vscode.cmake-tools` | CMake Tools | CMake 项目集成、构建、调试 |
| `twxs.cmake` | CMake | CMakeLists.txt 语法高亮 |
| `ms-vscode.cpptools-extension-pack` | C/C++ Extension Pack | C++ 开发工具合集 |

### 3.2 工作区设置

在项目根目录创建 `.vscode/settings.json`：

```json
{
    "cmake.generator": "Ninja",
    "cmake.configureArgs": [
        "-DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/mingw_64"
    ],
    "cmake.configureOnOpen": true,
    "C_Cpp.default.cppStandard": "c++17",
    "C_Cpp.default.intelliSenseMode": "windows-gcc-x64",
    "C_Cpp.default.compilerPath": "C:/Program Files/mingw64/bin/g++.exe"
}
```

### 3.3 推荐的工作流

1. **打开项目**: `File → Open Folder` 选择 `billiard/`
2. **CMake 自动配置**: 打开后 CMake Tools 会自动运行 configure
3. **选择构建目标**: 底部状态栏点击构建类型（Debug / Release）
4. **构建**: 按 `F7` 或点击底部状态栏 `Build`
5. **调试**: 按 `F5` 启动调试

---

## 四、项目构建命令 (命令行)

```powershell
# 克隆项目
git clone <repo-url> billiard
cd billiard

# Configure
cmake -G Ninja `
    -DCMAKE_PREFIX_PATH="C:/Qt/6.11.1/mingw_64" `
    -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
    -B build

# Build
cmake --build build

# 运行
.\build\billiard.exe
```

---

## 五、环境验证清单

全部通过即为配置成功：

- [ ] `g++ --version` → 14.x (MinGW)
- [ ] `cmake --version` → 4.x
- [ ] `ninja --version` → 1.13+
- [ ] `qmake6 --version` → Qt 6.11.1
- [ ] `vcpkg --version` → 正常输出版本号
- [ ] `cmake -G Ninja -DCMAKE_PREFIX_PATH="C:/Qt/6.11.1/mingw_64" -B build_test` → Configure OK
- [ ] `cmake --build build_test` → Build OK

---

## 六、常见问题

### Q1: CMake 找不到 Qt6

**原因**: 未指定 `CMAKE_PREFIX_PATH`。

**解决**: 在 configure 时添加 `-DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/mingw_64`。

### Q2: `The CXX compiler identification is unknown`

**原因**: MinGW 不在 PATH 中。

**解决**: 确认 `g++.exe` 所在目录已加入系统 PATH。

### Q3: vcpkg 安装包时报 SSL 错误

**原因**: 网络代理或证书问题。

**解决**: 确保能正常访问 GitHub，或配置代理。

### Q4: CMake 生成器不工作

**原因**: CMake 版本过旧不支持 Ninja 或指定的生成器。

**解决**: 升级到 CMake 3.22+。确认 Ninja 已在 PATH 中。

### Q5: Vulkan headers 警告

**现象**: `Could NOT find WrapVulkanHeaders`

**影响**: 无。本项目为 2D 游戏，不需要 Vulkan。可忽略此警告。

---

## 七、版本对照参考

以下为本项目实际开发环境（复现时建议保持一致）：

| 组件 | 版本 | 安装路径 |
|------|------|----------|
| MinGW g++ | 14.2.0 | `C:\Program Files\mingw64` |
| CMake | 4.1.2 | `C:\Program Files\CMake` |
| Ninja | 1.13.2 | WinGet 自动管理 |
| Qt 6 | 6.11.1 | `C:\Qt\6.11.1\mingw_64` |
| vcpkg | 2026-05-27 | `D:\SomeApps\vcpkg` |
