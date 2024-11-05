#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "task.h"
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    //ustawienie aktualnej daty
    ui->dateInput->setDateTime(QDateTime::currentDateTime());

    //połączenie przycisków
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddTaskClicked);
    connect(ui->sortDateButton, &QPushButton::clicked, this, &MainWindow::onSortDateClicked);
    connect(ui->sortPriorityButton, &QPushButton::clicked, this, &MainWindow::onSortPriorityClicked);

    //połączenie kalendarza z funkcją onDateSelected
    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &MainWindow::onDateSelected);

    //wyświetlenie aktualnej daty na starcie
    onDateSelected(QDate::currentDate());
}

MainWindow::~MainWindow() {
    delete ui;
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
}

void MainWindow::onSortPriorityClicked() {
    taskManager.sortByPriority();
    updateTaskList();
}

void MainWindow::onSortDateClicked() {
    taskManager.sortByDueDate();
    updateTaskList();
}

void MainWindow::updateTaskList() {
    ui->taskList->clear();

    const auto& tasks = taskManager.getTasks();

    for (const auto& task : tasks) {
        QString taskString = QString("Tytuł: %1, Opis: %2, Priorytet: %3, Termin: %4")
                                 .arg(QString::fromStdString(task.getTitle()))
                                 .arg(QString::fromStdString(task.getDescription()))
                                 .arg(task.getPriority())
                                 .arg(QDateTime::fromSecsSinceEpoch(task.getDueDate()).toString("yyyy-MM-dd HH:mm"));
        ui->taskList->addItem(taskString);
    }
}

//funkcja wywoływana po wybraniu daty w kalendarzu
void MainWindow::onDateSelected(const QDate &date) {
    ui->dateInput->setDate(date); //ustawienie wybranej daty
    displayTasksForSelectedDate(date); //wyświetla zadanie TYLKO na wybraną datę
}

//wyświetla zadanie TYLKO na wybraną datę
void MainWindow::displayTasksForSelectedDate(const QDate &date) {
    ui->taskList->clear();
    const auto& tasks = taskManager.getTasks();

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
