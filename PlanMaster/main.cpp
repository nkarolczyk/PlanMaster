#include <QApplication>
#include <QTimer>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.showSplashScreen(); //wywołanie ekranu powitalnego
    QTimer::singleShot(3000, [&mainWindow]() { mainWindow.show(); }); //pokazanie okna po 3sek

    return app.exec();
}
