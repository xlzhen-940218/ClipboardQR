# Clipboard QR / 剪贴板二维码

[English](#english) | [中文](#中文)

---

<h2 id="english">English</h2>

### Description
**Clipboard QR** is a lightweight Windows desktop application written in C++ that automatically reads text from your clipboard and generates a QR code. It runs quietly in the system tray, making it incredibly easy to share links, text, or snippets from your PC to your mobile device.

### Features
* **Instant QR Generation:** Automatically grabs the current clipboard text and converts it to a QR code on launch.
* **System Tray Integration:** Clicking the "Close" button minimizes the app to the system tray instead of exiting.
* **Quick Refresh:** Double-click the system tray icon to instantly read the latest clipboard data and update the QR code.
* **UTF-8 Support:** Fully supports UTF-16 to UTF-8 conversion, ensuring accurate encoding for non-English characters.
* **Lightweight:** Native Win32 API implementation with minimal dependencies.

### Dependencies
* **C++ Compiler:** Supports C++11 or later (MSVC, MinGW/GCC).
* **Windows SDK:** For native Win32 API GUI and Clipboard access.
* **QR Code Generator:** Uses [Nayuki's QR Code generator](https://github.com/nayuki/QR-Code-generator).

### Build Instructions
Ensure you have `qrcodegen.hpp`, `qrcodegen.cpp`, your main C++ file, and an icon file (`qrcode.ico`) in your project directory. 

> **Note on App Icon:** To prevent an invisible/transparent system tray icon, you must compile a resource script (`.rc`) containing the icon.

**Using MinGW (GCC):**
```bash
# 1. Compile the resource file
windres resource.rc -o resource.o

# 2. Compile the application
g++ main.cpp qrcodegen.cpp resource.o -o ClipboardQR.exe -lgdi32 -mwindows
```

**Using Visual Studio (MSVC):**
1. Create a new "Windows Desktop Application" (C++) project.
2. Add `main.cpp`, `qrcodegen.hpp`, and `qrcodegen.cpp` to your source files.
3. Add a Resource File (`.rc`) and include your `.ico` file, ensuring its ID matches `IDI_ICON1` (101).
4. Build the solution.

---

<h2 id="中文">中文</h2>

### 简介
**剪贴板二维码 (Clipboard QR)** 是一款使用 C++ 编写的轻量级 Windows 桌面应用程序。它可以自动读取剪贴板中的文本并生成二维码。程序支持最小化到系统托盘静默运行，非常适合将电脑上的链接、文本或代码片段快速分享到移动设备。

### 功能特性
* **即时生成：** 启动时自动读取当前剪贴板文本并渲染为二维码。
* **系统托盘集成：** 点击窗口的“关闭”按钮会隐藏窗口并将其最小化到系统托盘，而非直接退出。
* **快速刷新：** 双击系统托盘图标，程序会立刻唤醒并读取最新的剪贴板内容，刷新二维码。
* **UTF-8 支持：** 完善的宽字符 (UTF-16) 到 UTF-8 转换逻辑，完美支持中文、日文、Emoji 等多语言字符。
* **轻量高效：** 采用原生 Win32 API 编写，资源占用极低。

### 依赖项
* **C++ 编译器：** 支持 C++11 或更高版本 (MSVC, MinGW/GCC)。
* **Windows SDK：** 用于原生 Win32 API 界面及剪贴板操作。
* **二维码生成库：** 使用了 [Nayuki 的 QR Code generator](https://github.com/nayuki/QR-Code-generator)。

### 编译指南
请确保你的项目目录中包含 `qrcodegen.hpp`、`qrcodegen.cpp`、主程序源码以及一个图标文件 (`qrcode.ico`)。

> **关于程序图标的重要提示：** 为了防止系统托盘图标变成透明或不可见，必须编译包含图标的资源文件 (`.rc`)。

**使用 MinGW (GCC) 编译：**
```bash
# 1. 编译资源文件
windres resource.rc -o resource.o

# 2. 编译主程序 (-mwindows 参数用于隐藏控制台黑框)
g++ main.cpp qrcodegen.cpp resource.o -o ClipboardQR.exe -lgdi32 -mwindows
```

**使用 Visual Studio (MSVC) 编译：**
1. 创建一个新的“Windows 桌面应用程序” (C++) 项目。
2. 将 `main.cpp`、`qrcodegen.hpp` 和 `qrcodegen.cpp` 添加到源文件。
3. 添加一个资源文件 (`.rc`) 并引入你的 `.ico` 图标，确保其资源 ID 与代码中的 `IDI_ICON1` (101) 匹配。
4. 生成解决方案。
