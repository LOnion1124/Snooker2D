#include "App.h"
#include "MainWindow.h"
#include "GameSessionViewModel.h"
#include "ContractsInit.h"

int Snooker2D::App::run(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // 注册自定义类型到 Qt 元对象系统（一次性）
    registerContractTypes();

    GameSessionViewModel sessionViewModel;
    MainWindow mainWindow;

    mainWindow.bindAll(sessionViewModel.bus());
    sessionViewModel.start();
    mainWindow.show();

    return app.exec();
}
