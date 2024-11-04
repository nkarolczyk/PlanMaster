#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "task.h"
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);  //musi być wywołane jako pierwsze! ale nie jest

    //połączenia sygnałów przycisków z odpowiednimi slotami
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddTaskClicked);
    connect(ui->sortDateButton, &QPushButton::clicked, this, &MainWindow::onSortDateClicked);
    connect(ui->sortPriorityButton, &QPushButton::clicked, this, &MainWindow::onSortPriorityClicked);

    qDebug() << "MainWindow zainicjalizowany z taskManager";
}


MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onAddTaskClicked() {
    qDebug() << "Kliknięto przycisk 'Dodaj Zadanie'";  ///nie dziala tu nic i tak na razie

    //pobranie danych z pól wejściowych
    QString title = ui->titleInput->text();
    QString description = ui->descriptionInput->toPlainText();
    int priority = ui->priorityInput->value();
    QDateTime dueDate = ui->dateInput->dateTime();

    qDebug() << "Dane zadania:" << title << description << priority << dueDate.toString();  //debugowanie danych wejściowych

    if (title.isEmpty() || description.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Pole tytułu i opisu nie mogą być puste.");
        return;
    }

    //tworzenie nowego zadania i dodawanie go do TaskManagera
    Task newTask(title.toStdString(), description.toStdString(), priority, dueDate.toSecsSinceEpoch());
    taskManager.addTask(newTask);

    qDebug() << "Zadanie dodane do TaskManagera";  // Informacja o dodaniu zadania

    //aktualizacja listy zadań po dodaniu nowego zadania
    updateTaskList();
}


void MainWindow::onSortPriorityClicked() {
    //sortowanie zadań według priorytetu
    taskManager.sortByPriority();
    updateTaskList();
}

void MainWindow::onSortDateClicked() {
    //sortowanie zadań według terminu
    taskManager.sortByDueDate();
    updateTaskList();
}

void MainWindow::updateTaskList() {
    //wyczyść listę przed dodaniem nowych elementów
    ui->taskList->clear();

    //pobierz zaktualizowaną listę zadań z TaskManagera
    const auto& tasks = taskManager.getTasks();

    //dodaj każde zadanie do listy
    for (const auto& task : tasks) {
        QString taskString = QString("Tytuł: %1, Opis: %2, Priorytet: %3, Termin: %4")
                                 .arg(QString::fromStdString(task.getTitle()))
                                 .arg(QString::fromStdString(task.getDescription()))
                                 .arg(task.getPriority())
                                 .arg(QDateTime::fromSecsSinceEpoch(task.getDueDate()).toString("yyyy-MM-dd HH:mm"));
        ui->taskList->addItem(taskString);
    }
}
