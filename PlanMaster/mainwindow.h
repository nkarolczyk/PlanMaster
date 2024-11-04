#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private:
    Ui::MainWindow *ui;
    TaskManager taskManager;
    void updateTaskList();  // Aktualizuje wyświetlaną listę zadań
};

#endif // MAINWINDOW_H
