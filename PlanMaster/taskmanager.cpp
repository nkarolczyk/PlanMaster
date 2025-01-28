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
    qDebug() << "Dodano zadanie:" << QString::fromStdString(task.getTitle())
             << "Priorytet:" << task.getPriority()
             << "Data:" << QDateTime::fromSecsSinceEpoch(task.getDueDate()).toString();
}

void TaskManager::removeTask(const std::string &title) {
    auto it = std::remove_if(tasks.begin(), tasks.end(), [&title](const Task &task) {
        return task.getTitle() == title;
    });
    if (it != tasks.end()) {
        tasks.erase(it, tasks.end());
        qDebug() << "Usunięto zadanie o tytule:" << QString::fromStdString(title);
    } else {
        qDebug() << "Zadanie o tytule:" << QString::fromStdString(title) << "nie znaleziono.";
    }
}

void TaskManager::markTaskAsCompleted(const std::string &title) {
    for (auto &task : tasks) {
        if (task.getTitle() == title) {
            task.markAsCompleted();
            qDebug() << "Zadanie oznaczone jako zakończone:" << QString::fromStdString(title);
            return;
        }
    }
    qDebug() << "Zadanie o tytule:" << QString::fromStdString(title) << "nie znaleziono.";
}

void TaskManager::displayTasks() const {
    if (tasks.empty()) {
        qDebug() << "Lista zadań jest pusta.";
        return;
    }

    for (const auto &task : tasks) {
        qDebug() << "Tytuł:" << QString::fromStdString(task.getTitle())
                 << "Priorytet:" << task.getPriority()
                 << "Data:" << QDateTime::fromSecsSinceEpoch(task.getDueDate()).toString()
                 << "Zakończone:" << (task.getIsCompleted() ? "Tak" : "Nie");
    }
}

void TaskManager::sortByPriority() {
    if (tasks.empty()) {
        qDebug() << "Nie można posortować zadań - lista jest pusta.";
        return;
    }

    qDebug() << "Rozpoczęto sortowanie zadań według priorytetu.";
    std::sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b) {
        return a.getPriority() > b.getPriority();
    });
    qDebug() << "Zakończono sortowanie zadań według priorytetu.";
    displayTasks(); // Wyświetla zadania po sortowaniu
}


void TaskManager::sortByDueDate() {
    std::sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b) {
        return a.getDueDate() < b.getDueDate();
    });
    qDebug() << "Zadania posortowane według daty";
}

const std::vector<Task>& TaskManager::getTasks() const {
    return tasks;
}

void TaskManager::saveToFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Błąd podczas zapisu do pliku:" << filename;
        return;
    }

    QJsonArray jsonArray;
    for (const auto &task : tasks) {
        qDebug() << "Tytuł:" << QString::fromStdString(task.getTitle())
                 << "Priorytet:" << task.getPriority();
    }

    QJsonDocument doc(jsonArray);
    file.write(doc.toJson());
    file.close();
    qDebug() << "Zapisano zadania do pliku JSON:" << filename;
}

void TaskManager::loadFromFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Błąd podczas wczytywania pliku:" << filename;
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        qDebug() << "Błędny format pliku JSON:" << filename;
        return;
    }

    QJsonArray jsonArray = doc.array();
    tasks.clear();
    for (const auto &value : jsonArray) {
        if (value.isObject()) {
            tasks.push_back(Task::fromJson(value.toObject()));
        }
    }

    qDebug() << "Wczytano zadania z pliku JSON:" << filename;
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
        out << "Zakończone: " << (task.getIsCompleted() ? "Tak" : "Nie") << "\n";
        out << "---------------------------------\n";
    }
    file.close();
    qDebug() << "Zadania zostały zapisane do pliku TXT:" << filename;
}

void TaskManager::updateTask(const Task &task) {
    for (auto &t : tasks) {
        if (t.getTitle() == task.getTitle()) {
            t = task;
            break;
        }
    }
}

void TaskManager::clearTasks() {
    tasks.clear();
    qDebug() << "Wyczyszczono listę zadań.";
}

QMap<QString, int> TaskManager::getTaskStatistics() const {
    int completedCount = 0;
    int ongoingCount = 0;

    for (const auto &task : tasks) {
        if (task.getIsCompleted()) {
            ++completedCount;
        } else {
            ++ongoingCount;
        }
    }

    QMap<QString, int> stats;
    stats["Completed"] = completedCount;
    stats["Ongoing"] = ongoingCount;
    return stats;
}

