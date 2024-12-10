#include "logindialog.h"
#include "ui_logindialog.h"
#include "registerdialog.h"
#include "databasemanager.h"
#include <QMessageBox>
#include "mainwindow.h"


LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog) {
    ui->setupUi(this);
}

LoginDialog::~LoginDialog() {
    delete ui;
}

QString LoginDialog::getUsername() const {
    return ui->usernameLineEdit->text();
}

QString LoginDialog::getPassword() const {
    return ui->passwordLineEdit->text();
}

void LoginDialog::on_registerButton_clicked() {
    RegisterDialog registerDialog(this);
    if (registerDialog.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "Rejestracja", "Zarejestrowano pomyślnie. Teraz możesz się zalogować.");
        ui->usernameLineEdit->setText(registerDialog.getUsername());
    }
}

void LoginDialog::on_loginButton_clicked() {
    QString username = getUsername();
    QString password = getPassword();

    DatabaseManager dbManager;
    qDebug() << "Login próbny: Username:" << username << "Password:" << password;

    if (dbManager.authenticate(username, password)) {
        accept(); //zamykamy okno logowania po udanym logowaniu

        //kalendarzyk
        MainWindow *mainWindow = new MainWindow(dbManager.getUserId(username, password), nullptr);

        mainWindow->show();
    } else {
        QMessageBox::warning(this, "Błąd logowania", "Nieprawidłowy login lub hasło.");
    }
}

