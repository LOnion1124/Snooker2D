#pragma once

#include <QApplication>

namespace Snooker2D {

class GameSessionViewModel;
class MainWindow;

// 装配 ViewModel、MainWindow 和 Bus，启动事件循环
class App {
public:
    int run(int argc, char* argv[]);
};

} // namespace Snooker2D
