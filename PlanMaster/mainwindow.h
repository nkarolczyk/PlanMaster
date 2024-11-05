#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include <QDateTime>
#include "taskmanager.h"

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddTaskClicked();
    void onSortPriorityClicked();
    void onSortDateClicked();
    void onDateSelected(const QDate &date);

private:
    Ui::MainWindow *ui;
    TaskManager taskManager;
    void updateTaskList();
    void displayTasksForSelectedDate(const QDate &date);
};

#endif
