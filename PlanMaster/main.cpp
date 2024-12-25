#include "logindialog.h"
#include "mainwindow.h"
#include "databasemanager.h"
#include <QApplication>
#include <QDebug>
#include <QTimer>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    DatabaseManager dbManager;
    if (!dbManager.openDatabase()) {
        qDebug() << "Nie udało się otworzyć bazy danych. Aplikacja zostanie zamknięta.";
        return -1;
    }

    LoginDialog loginDialog;
    if (loginDialog.exec() == QDialog::Accepted) {
        QString username = loginDialog.getUsername();
        QString password = loginDialog.getPassword();

        if (dbManager.authenticate(username, password)) {
            int userId = dbManager.getUserId(username, password);
            if (userId != -1) {
                MainWindow *mainWindow = new MainWindow(userId);
                mainWindow->showSplashScreen();

                QTimer::singleShot(3000, [mainWindow]() { mainWindow->show(); });

                return app.exec();
            } else {
                qDebug() << "Błąd: Nie znaleziono identyfikatora użytkownika.";
            }
        } else {
            qDebug() << "Błąd logowania: Nieprawidłowe dane logowania.";
        }
    }

    dbManager.closeDatabase();

    return 0;
}
