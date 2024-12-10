#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "databasemanager.h"
#include <QMessageBox>

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::RegisterDialog) {
    ui->setupUi(this);
}

RegisterDialog::~RegisterDialog() {
    delete ui;
}

QString RegisterDialog::getUsername() const {
    return ui->usernameLineEdit->text();
}

QString RegisterDialog::getPassword() const {
    return ui->passwordLineEdit->text();
}

void RegisterDialog::on_registerButton_clicked() {
    QString username = getUsername();
    QString password = getPassword();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Uzupełnij wszystkie pola.");
        return;
    }

    DatabaseManager dbManager;
    if (dbManager.addUser(username, password)) {
        QMessageBox::information(this, "Sukces", "Rejestracja zakończona pomyślnie!");
        accept();
    } else {
        QMessageBox::warning(this, "Błąd", "Rejestracja nie powiodła się. Sprawdź logi.");
    }
}
