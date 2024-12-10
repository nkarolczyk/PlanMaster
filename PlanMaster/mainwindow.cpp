#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QSplashScreen>
#include <QPixmap>

MainWindow::MainWindow(int userId, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), userId(userId) {
    ui->setupUi(this);

    // Wyświetlenie splash screena
    showSplashScreen();

    // Ustawienie aktualnej daty
    ui->dateInput->setDateTime(QDateTime::currentDateTime());

    //połączenie guzików ze slotami
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddTaskClicked);
    connect(ui->sortDateButton, &QPushButton::clicked, this, &MainWindow::onSortDateClicked);
    connect(ui->sortPriorityButton, &QPushButton::clicked, this, &MainWindow::onSortPriorityClicked);
    connect(ui->btnExportToTxt, &QPushButton::clicked, this, &MainWindow::onExportTasksClicked);
    connect(ui->btnRemoveTask, &QPushButton::clicked, this, &MainWindow::onRemoveTaskClicked);
    connect(ui->btnCompleteTask, &QPushButton::clicked, this, &MainWindow::onCompleteTaskClicked);

    updateTaskList();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::showSplashScreen() {
    QSplashScreen *splash = new QSplashScreen(QPixmap(":/images/splash.png"));
    splash->show();
    QTimer::singleShot(2000, splash, &QSplashScreen::close);
}

void MainWindow::onAddTaskClicked() {
    QString title = ui->titleInput->text();
    QDateTime dueDate = ui->dateInput->dateTime();
    int priority = ui->priorityInput->value();

    if (title.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Tytuł zadania nie może być pusty.");
        return;
    }

    if (dbManager.addTask(userId, title, dueDate, priority)) {
        updateTaskList();
        QMessageBox::information(this, "Sukces", "Zadanie zostało dodane.");
    } else {
        QMessageBox::warning(this, "Błąd", "Nie udało się dodać zadania.");
    }
}

//JESZCZE NIE DZIAŁA - JEST BŁĄD

void MainWindow::onRemoveTaskClicked() {
    QListWidgetItem *selectedItem = ui->taskList->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "Błąd", "Wybierz zadanie do usunięcia.");
        return;
    }

    QString fullText = selectedItem->text();
    QString title = fullText.split(" - Due: ").at(0);
    title = title.split("] ").last();

    if (dbManager.removeTask(title)) {
        updateTaskList();
        QMessageBox::information(this, "Sukces", "Zadanie zostało usunięte.");
    } else {
        QMessageBox::warning(this, "Błąd", "Nie udało się usunąć zadania.");
    }
}

void MainWindow::onCompleteTaskClicked() {
    QListWidgetItem *selectedItem = ui->taskList->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "Błąd", "Wybierz zadanie do oznaczenia jako zakończone.");
        return;
    }

    QString fullText = selectedItem->text();
    QString title = fullText.split(" - Due: ").at(0);
    title = title.split("] ").last();

    if (dbManager.markTaskAsCompleted(title)) {
        updateTaskList();
        QMessageBox::information(this, "Sukces", "Zadanie zostało oznaczone jako zakończone.");
    } else {
        QMessageBox::warning(this, "Błąd", "Nie udało się oznaczyć zadania jako zakończone.");
    }
}

void MainWindow::onSortDateClicked() {
    updateTaskList();  // Sortowanie po dacie
}

void MainWindow::onSortPriorityClicked() {
    ui->taskList->clear();
    QList<QString> tasks = dbManager.getTasksSortedByPriority(userId);
    if (tasks.isEmpty()) {
        ui->taskList->addItem("Brak zadań.");
    } else {
        for (const QString &task : tasks) {
            ui->taskList->addItem(task);
        }
    }
}

void MainWindow::onExportTasksClicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Zapisz jako", "", "Pliki tekstowe (*.txt)");
    if (fileName.isEmpty()) return;

    if (dbManager.exportToTextFile(userId, fileName)) {
        QMessageBox::information(this, "Sukces", "Zadania zostały wyeksportowane.");
    } else {
        QMessageBox::warning(this, "Błąd", "Nie udało się wyeksportować zadań.");
    }
}

void MainWindow::updateTaskList() {
    ui->taskList->clear();
    QList<QString> tasks = dbManager.getTasksForUser(userId);

    qDebug() << "Odświeżenie listy zadań. Ilość zadań:" << tasks.size();

    if (tasks.isEmpty()) {
        ui->taskList->addItem("Brak zadań do wyświetlenia.");
    } else {
        for (const QString &task : tasks) {
            ui->taskList->addItem(task);
        }
    }
}

void MainWindow::checkReminders() {
    //przypomnienia maja byc
}

void MainWindow::onDateSelected(const QDate &date) {
    ui->dateInput->setDate(date);
    displayTasksForSelectedDate(date);
}

void MainWindow::displayTasksForSelectedDate(const QDate &date) {
    ui->taskList->clear();

    QList<QString> tasks = dbManager.getTasksForUser(userId);
    if (tasks.isEmpty()) {
        ui->taskList->addItem("Brak zadań dla wybranej daty.");
        return;
    }

    bool found = false;
    for (const QString &taskDescription : tasks) {
        //"[Priority: X] Tytuł - Due: RRRR-MM-DDTHH:MM:SS"
        QString duePart = taskDescription.split("- Due: ").last();
        QDateTime taskDateTime = QDateTime::fromString(duePart, Qt::ISODate);
        if (taskDateTime.date() == date) {
            ui->taskList->addItem(taskDescription);
            found = true;
        }
    }

    if (!found) {
        ui->taskList->addItem("Brak zadań dla wybranej daty.");
    }
}
