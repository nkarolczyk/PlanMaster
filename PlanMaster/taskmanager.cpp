#include "taskmanager.h"
#include <algorithm>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

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
                 << ", Termin:" << QString::fromStdString(
                        QDateTime::fromSecsSinceEpoch(task.getDueDate()).toString().toStdString());
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

void TaskManager::saveToFile(const QString &filename) {
    QJsonArray taskArray;
    for (const auto &task : tasks) {
        QJsonObject taskObj;
        taskObj["title"] = QString::fromStdString(task.getTitle());
        taskObj["description"] = QString::fromStdString(task.getDescription());
        taskObj["priority"] = task.getPriority();
        taskObj["dueDate"] = static_cast<qint64>(task.getDueDate());
        taskArray.append(taskObj);
    }
    QJsonDocument doc(taskArray);
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        qDebug() << "Zadania zapisano do pliku JSON:" << filename;
    } else {
        qDebug() << "Błąd zapisu do pliku JSON:" << filename;
    }
}

void TaskManager::loadFromFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Błąd odczytu pliku JSON:" << filename;
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray taskArray = doc.array();
    for (const auto &item : taskArray) {
        QJsonObject taskObj = item.toObject();
        Task task(taskObj["title"].toString().toStdString(),
                  taskObj["description"].toString().toStdString(),
                  taskObj["priority"].toInt(),
                  static_cast<std::time_t>(taskObj["dueDate"].toVariant().toLongLong()));
        tasks.push_back(task);
    }
    qDebug() << "Zadania wczytano z pliku JSON:" << filename;
}

void TaskManager::exportToTextFile(const QString &filename) const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Błąd podczas otwierania pliku TXT do zapisu:" << filename;
        return;
    }

    QTextStream out(&file);
    out << "Lista zadań:\n";

    for (const auto &task : tasks) {
        out << "Tytuł: " << QString::fromStdString(task.getTitle()) << "\n";
        out << "Opis: " << QString::fromStdString(task.getDescription()) << "\n";
        out << "Priorytet: " << task.getPriority() << "\n";
        out << "Termin: " << QDateTime::fromSecsSinceEpoch(task.getDueDate()).toString() << "\n";
        out << "---------------------------------\n";
    }

    file.close();
    qDebug() << "Zadania zostały zapisane do pliku TXT:" << filename;
}
