#include <QApplication>
#include "app/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Snooker2D::MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
