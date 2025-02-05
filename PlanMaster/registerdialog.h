#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog {
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

    QString getUsername() const;
    QString getPassword() const;

private slots:
    void on_registerButton_clicked();

private:
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H
