#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "task.h"
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QSplashScreen>
#include <QTimer>
#include <QCoreApplication>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Wyświetlenie splash screena
    showSplashScreen();

    // Ustawienie aktualnej daty
    ui->dateInput->setDateTime(QDateTime::currentDateTime());

    // Połączenie przycisków
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddTaskClicked);
    connect(ui->sortDateButton, &QPushButton::clicked, this, &MainWindow::onSortDateClicked);
    connect(ui->sortPriorityButton, &QPushButton::clicked, this, &MainWindow::onSortPriorityClicked);
    connect(ui->btnExportToTxt, &QPushButton::clicked, this, &MainWindow::onExportTasksClicked); // Połączenie przycisku eksportu

    // Połączenie kalendarza z funkcją onDateSelected
    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &MainWindow::onDateSelected);

    // Wyświetlenie aktualnej daty na starcie
    onDateSelected(QDate::currentDate());
}

MainWindow::~MainWindow() {
    delete ui;
}

// Implementacja metody wyświetlającej splash screen
void MainWindow::showSplashScreen() {
    QSplashScreen *splash = new QSplashScreen(QPixmap(":/images/splash_image.png"));
    if (QPixmap(":/images/splash_image.png").isNull()) {
        qDebug() << "Nie można załadować obrazu splash screena!";
        return;
    }
    splash->show();

    QTimer::singleShot(3000, [splash, this]() {
        splash->close();
        this->show();
    });
}

void MainWindow::onAddTaskClicked() {
    qDebug() << "Kliknięto przycisk 'Dodaj Zadanie'";

    QString title = ui->titleInput->text();
    QString description = ui->descriptionInput->toPlainText();
    int priority = ui->priorityInput->value();
    QDateTime dueDate = ui->dateInput->dateTime();

    if (title.isEmpty() || description.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Pole tytułu i opisu nie mogą być puste.");
        return;
    }

    Task newTask(title.toStdString(), description.toStdString(), priority, dueDate.toSecsSinceEpoch());
    taskManager.addTask(newTask);

    updateTaskList();
    QMessageBox::information(this, "Sukces", "Zadanie zostało dodane pomyślnie!");
}

void MainWindow::onSortPriorityClicked() {
    taskManager.sortByPriority();
    updateTaskList();
}

void MainWindow::onSortDateClicked() {
    taskManager.sortByDueDate();
    updateTaskList();
}

void MainWindow::onExportTasksClicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Zapisz plik tekstowy", "", "Pliki tekstowe (*.txt)");

    if (!fileName.isEmpty()) {
        taskManager.exportToTextFile(fileName);
        QMessageBox::information(this, "Sukces", "Zadania zostały zapisane do pliku tekstowego!");
    }
}

void MainWindow::updateTaskList() {
    ui->taskList->clear();

    const auto& tasks = taskManager.getTasks();

    if (tasks.empty()) {
        ui->taskList->addItem("Brak zadań do wyświetlenia.");
        return;
    }

    for (const auto& task : tasks) {
        QString taskString = QString("Tytuł: %1, Opis: %2, Priorytet: %3, Termin: %4")
                                 .arg(QString::fromStdString(task.getTitle()))
                                 .arg(QString::fromStdString(task.getDescription()))
                                 .arg(task.getPriority())
                                 .arg(QDateTime::fromSecsSinceEpoch(task.getDueDate()).toString("yyyy-MM-dd HH:mm"));
        ui->taskList->addItem(taskString);
    }
}

void MainWindow::onDateSelected(const QDate &date) {
    ui->dateInput->setDate(date); // Ustawienie wybranej daty
    displayTasksForSelectedDate(date); // Wyświetla zadanie TYLKO na wybraną datę
}

void MainWindow::displayTasksForSelectedDate(const QDate &date) {
    ui->taskList->clear();
    const auto& tasks = taskManager.getTasks();

    if (tasks.empty()) {
        ui->taskList->addItem("Brak zadań do wyświetlenia.");
        return;
    }

    for (const auto& task : tasks) {
        QDateTime taskDateTime = QDateTime::fromSecsSinceEpoch(task.getDueDate());
        if (taskDateTime.date() == date) {
            QString taskString = QString("Tytuł: %1, Opis: %2, Priorytet: %3, Termin: %4")
                                     .arg(QString::fromStdString(task.getTitle()))
                                     .arg(QString::fromStdString(task.getDescription()))
                                     .arg(task.getPriority())
                                     .arg(taskDateTime.toString("yyyy-MM-dd HH:mm"));
            ui->taskList->addItem(taskString);
        }
    }
}
