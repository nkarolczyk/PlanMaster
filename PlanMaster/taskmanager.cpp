#include "taskmanager.h"
#include <algorithm>
#include <QDebug>

void TaskManager::addTask(const Task &task) {
    tasks.push_back(task);
    qDebug() << "Dodano zadanie:" << QString::fromStdString(task.getTitle());
}

void TaskManager::removeTask(const std::string &title) {
    auto it = std::remove_if(tasks.begin(), tasks.end(),
                             [&title](const Task &task) { return task.getTitle() == title; });
    if (it != tasks.end()) {
        tasks.erase(it, tasks.end());
        qDebug() << "Usunięto zadanie o tytule:" << QString::fromStdString(title);
    } else {
        qDebug() << "Zadanie o tytule:" << QString::fromStdString(title) << "nie znaleziono.";
    }
}

void TaskManager::displayTasks() const {
    for (const auto &task : tasks) {
        qDebug() << "Tytuł:" << QString::fromStdString(task.getTitle())
                 << ", Opis:" << QString::fromStdString(task.getDescription())
                 << ", Priorytet:" << task.getPriority()
                 << ", Termin:" << QString::number(task.getDueDate());
    }
}

void TaskManager::sortByPriority() {
    std::sort(tasks.begin(), tasks.end(),
              [](const Task &a, const Task &b) { return a.getPriority() > b.getPriority(); });
    qDebug() << "Zadania posortowane według priorytetu";
}

void TaskManager::sortByDueDate() {
    std::sort(tasks.begin(), tasks.end(),
              [](const Task &a, const Task &b) { return a.getDueDate() < b.getDueDate(); });
    qDebug() << "Zadania posortowane według daty";
}

const std::vector<Task>& TaskManager::getTasks() const {
    return tasks;
}
