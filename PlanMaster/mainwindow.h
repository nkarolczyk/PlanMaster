#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QDateTime>
#include <QSystemTrayIcon>
#include <QTimer>
#include "databasemanager.h"
#include <QSplashScreen>
#include <QPixmap>
#include "taskmanager.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(int userId, QWidget *parent = nullptr);
    ~MainWindow();

    void showSplashScreen();

private slots:
    //przyciski
    void onAddTaskClicked();
    void onSortPriorityClicked();
    void onSortDateClicked();
    void onExportTasksClicked();
    void onDateSelected(const QDate &date);
    void onRemoveTaskClicked();
    void checkReminders();

    void on_btnAbout_clicked();

    void on_btnHome_clicked();

    void on_btnAnalytics_clicked();

    void on_btnCalendar_clicked();

    void on_btnPlanMaster_clicked();


    void on_btnPlanMaster2_clicked();

    void onTaskClicked(QListWidgetItem *item);

    void onCalendarDateForAdding(const QDate &date);

    void checkDeadlines();
    void onTaskItemClicked(QListWidgetItem *item);
    void on_btnCompleteTask_clicked();

    void on_btnDefaultView_clicked();

    void onCalendarDateSelected(const QDate &date);
    void displayWeeklyTasks(const QDate &startOfWeek);
    void setupCalendarView();


    void on_btnAddTaskToDate_clicked();

    void on_btnDefaultView_2_clicked();

    void displayTasksForSelectedDate(const QDate &date);





private:
    Ui::MainWindow *ui;             //wskaźnik na interfejs użytkownika
    QSystemTrayIcon *trayIcon;      //ikona w zasobniku systemowym
    QTimer *reminderTimer;          //timer do przypomnień
    int userId;                     //identyfikator użytkownika
    DatabaseManager dbManager;      //obiekt do zarządzania bazą danych

    void updateTaskList();                            //aktualizacja listy zadań
    void showReminderNotification(const QString &message); //powiadomienia
    void updateAnalytics(); //analiza zadan

    TaskManager *taskManager;
    int findNextFreeRow(int column);
    void updateAnalyticsView();
    void syncTasksWithDatabase();
    void openTaskInCalendar(const Task &task);

    void updateCalendarView();
    void updateTaskListForDay(const QDate &date);


};

#endif // MAINWINDOW_H
