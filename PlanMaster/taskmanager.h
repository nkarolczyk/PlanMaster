#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "task.h"
#include <vector>
#include <QString>
#include <QFile>
#include <QDateTime>

class TaskManager {
private:
    std::vector<Task> tasks;

public:
    void addTask(const Task &task);
    void removeTask(const std::string &title);
    void markTaskAsCompleted(const std::string &title);
    void displayTasks() const;
    void sortByPriority();
    void sortByDueDate();
    const std::vector<Task>& getTasks() const;
    void saveToFile(const QString &filename);
    void loadFromFile(const QString &filename);
    void exportToTextFile(const QString &filename) const;
    void updateTask(const Task &task);
    void clearTasks();
    QMap<QString, int> getTaskStatistics() const;




};

#endif // TASKMANAGER_H
