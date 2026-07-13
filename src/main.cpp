#include <QApplication>

// View
#include "view/MainWindow.h"

// ViewModel（App 层负责创建 ViewModel 并注入 Bus）
#include "viewmodel/GameSessionViewModel.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // App 层：创建 ViewModel，获取通信总线
    Snooker2D::GameSessionViewModel sessionViewModel;

    // View 层：创建主窗口，通过 Bus 与 ViewModel 解耦
    Snooker2D::MainWindow mainWindow;
    mainWindow.init(sessionViewModel.bus());

    // 启动游戏逻辑
    sessionViewModel.start();

    mainWindow.show();
    return app.exec();
}
