#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QCryptographicHash>
#include <QMessageBox>

QString DatabaseManager::hashPassword(const QString &password) {
    QByteArray passwordBytes = password.toUtf8();
    QByteArray hashedPassword = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);
    return QString(hashedPassword.toHex());

}

DatabaseManager::DatabaseManager() {
    if (QSqlDatabase::contains("unique_connection_name")) {
        db = QSqlDatabase::database("unique_connection_name");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", "unique_connection_name");
        db.setDatabaseName("tasks.db");
    }
}

QList<QString> DatabaseManager::getTasksSortedByPriority(int userId) {
    QList<QString> tasks;
    if (!db.isOpen()) return tasks;

    QSqlQuery query(db);
    query.prepare("SELECT title, dueDate, priority, description FROM tasks WHERE user_id = :user_id ORDER BY priority DESC");
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qWarning() << "Błąd sortowania zadań według priorytetu:" << query.lastError().text();
        return tasks;
    }

    while (query.next()) {
        QString task = QString("%1|%2|%3|%4")
                           .arg(query.value("title").toString())
                           .arg(query.value("dueDate").toString())
                           .arg(query.value("priority").toInt())
                           .arg(query.value("description").toString());
        tasks.append(task);
    }

    return tasks;
}


bool DatabaseManager::addTask(int userId,
                              const QString &title,
                              const QDateTime &dueDate,
                              int priority,
                              const QString &description,
                              bool completed) {
    if (!db.isOpen()) {
        qWarning() << "Baza danych nie jest otwarta.";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO Tasks (user_id, title, dueDate, priority, description, completed) "
                  "VALUES (:user_id, :title, :dueDate, :priority, :description, :completed)");
    query.bindValue(":user_id", userId);
    query.bindValue(":title", title);
    query.bindValue(":dueDate", dueDate.toString("yyyy-MM-dd HH:mm:ss"));
    query.bindValue(":priority", priority);
    query.bindValue(":description", description);
    query.bindValue(":completed", completed ? 1 : 0);


    if (!query.exec()) {
        qWarning() << "Błąd podczas dodawania zadania:" << query.lastError().text();
        return false;
    }

    qDebug() << "Dodano zadanie:" << title;
    return true;
}



bool DatabaseManager::removeTask(const QString &title) {
    if (!openDatabase()) return false;

    QSqlQuery query(db);
    query.prepare("DELETE FROM tasks WHERE title = :title");
    query.bindValue(":title", title);

    if (!query.exec()) {
        qWarning() << "Failed to remove task:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::markTaskAsCompleted(int userId, const QString &title)
{
    if (!db.isOpen()) {
        qWarning() << "Baza danych nie jest otwarta.";
        return false;
    }

    QSqlQuery query(db);
    //completed=1, ale TYLKO dla zadania z danym user_id i tytułem
    query.prepare("UPDATE tasks SET completed = 1 WHERE user_id = :user_id AND title = :title");
    query.bindValue(":user_id", userId);
    query.bindValue(":title", title);

    if (!query.exec()) {
        qWarning() << "Błąd w markTaskAsCompleted:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::authenticate(const QString &username, const QString &password) {
    if (!openDatabase()) return false;

    QString hashedPassword = hashPassword(password);

    QSqlQuery query(db);
    query.prepare("SELECT id FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", hashedPassword);

    if (!query.exec()) {
        qWarning() << "Query execution failed:" << query.lastError().text();
        return false;
    }

    return query.next(); //czy istnieje dopasowany rekord
}



int DatabaseManager::getUserId(const QString &username, const QString &password) {
    if (!openDatabase()) return -1;

    QSqlQuery query(db);
    query.prepare("SELECT id FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", hashPassword(password));

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    } else {
        qWarning() << "Failed to retrieve user ID:" << query.lastError().text();
        return -1;
    }
}

bool DatabaseManager::addUser(const QString &username, const QString &password) {
    if (!openDatabase()) return false;

    // Sprawdzenie, czy użytkownik już istnieje
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    checkQuery.bindValue(":username", username);

    if (!checkQuery.exec()) {
        qWarning() << "Query execution failed:" << checkQuery.lastError().text();
        return false;
    }

    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        qWarning() << "Użytkownik o tej nazwie już istnieje.";
        return false;
    }

    QString hashedPassword = hashPassword(password);

    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    insertQuery.bindValue(":username", username);
    insertQuery.bindValue(":password", hashedPassword);

    if (!insertQuery.exec()) {
        qWarning() << "Failed to add user:" << insertQuery.lastError().text();
        return false;
    }

    qDebug() << "Dodano użytkownika:" << username;
    return true;
}

QList<QString> DatabaseManager::getTasksForUser(int userId) {
    QList<QString> tasks;

    if (!db.isValid()) {
        qWarning() << "Baza danych jest nieprawidłowa.";
        return tasks;
    }

    if (!db.isOpen() && !db.open()) {
        qWarning() << "Baza danych nie jest otwarta. Błąd:" << db.lastError().text();
        return tasks;
    }

    QSqlQuery query(db);
    query.prepare("SELECT title, dueDate, priority, description, completed FROM tasks WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qWarning() << "Błąd SELECT getTasksForUser:" << query.lastError().text();
        return tasks;
    }

    while (query.next()) {
        QString task = QString("%1|%2|%3|%4|%5")
                           .arg(query.value("title").toString())
                           .arg(query.value("dueDate").toString())
                           .arg(query.value("priority").toInt())
                           .arg(query.value("description").toString())
                           .arg(query.value("completed").toInt());
        tasks.append(task);
    }

    return tasks;
}


bool DatabaseManager::exportToTextFile(int userId, const QString &fileName) {
    if (!openDatabase()) return false;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for export:" << file.errorString();
        return false;
    }

    QTextStream out(&file);

    QSqlQuery query(db);
    query.prepare("SELECT title, dueDate, priority FROM tasks WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (query.exec()) {
        while (query.next()) {
            out << QString("[Priority: %1] %2 - Due: %3\n")
                       .arg(query.value(2).toInt())
                       .arg(query.value(0).toString())
                       .arg(query.value(1).toString());
        }
    } else {
        qWarning() << "Failed to export tasks:" << query.lastError().text();
        return false;
    }

    file.close();
    return true;
}

QMap<QString, int> DatabaseManager::getTaskStatistics(int userId) {
    QMap<QString, int> stats;

    if (!db.isOpen()) {
        qWarning() << "Baza danych nie jest otwarta.";
        return stats;
    }

    QSqlQuery query(db);
    query.prepare("SELECT completed, COUNT(*) FROM tasks WHERE user_id = :userId GROUP BY completed");
    query.bindValue(":userId", userId);

    if (query.exec()) {
        while (query.next()) {
            bool completed = query.value(0).toBool();
            int count = query.value(1).toInt();
            stats[completed ? "Ukończone" : "Nieukończone"] = count;
        }
    } else {
        qWarning() << "Błąd przy pobieraniu statystyk:" << query.lastError().text();
    }

    return stats;
}

QString DatabaseManager::getTaskById(int taskId)
{
    if (!db.isOpen()) {
        qWarning() << "Baza danych nie jest otwarta.";
        return "";
    }

    QSqlQuery query(db);
    // Również do description i completed
    query.prepare("SELECT id, title, dueDate, priority, description, completed "
                  "FROM Tasks WHERE id = :id");
    query.bindValue(":id", taskId);

    if (!query.exec()) {
        qWarning() << "Błąd w getTaskById:" << query.lastError().text();
        return "";
    }

    if (query.next()) {
        int id = query.value("id").toInt();
        QString title = query.value("title").toString();
        QString dDate = query.value("dueDate").toString();
        int prio = query.value("priority").toInt();
        QString desc = query.value("description").toString();
        int comp = query.value("completed").toInt();

        // "id|title|dueDate|priority|description|completed"
        return QString("%1|%2|%3|%4|%5|%6")
            .arg(id)
            .arg(title)
            .arg(dDate)
            .arg(prio)
            .arg(desc)
            .arg(comp);
    }
    // Gdy brak wiersza
    return "";
}

bool DatabaseManager::openDatabase() {
    if (!db.isValid()) {
        qWarning() << "Nieprawidłowe połączenie z bazą danych.";
        return false;
    }

    if (!db.isOpen()) {
        if (!db.open()) {
            qWarning() << "Nie udało się otworzyć bazy danych:" << db.lastError().text();
            return false;
        }
    }

    qDebug() << "Baza danych otwarta pomyślnie.";
    return true;
}


void DatabaseManager::closeDatabase() {
    if (db.isOpen()) {
        db.close();
        qDebug() << "Baza danych została zamknięta.";
    }
}

QList<QString> DatabaseManager::getTasksSortedByDate(int userId) {
    QList<QString> tasks;
    if (!db.isOpen()) return tasks;

    QSqlQuery query(db);
    query.prepare("SELECT title, dueDate, priority, description FROM tasks WHERE user_id = :user_id ORDER BY dueDate ASC");
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qWarning() << "Błąd sortowania zadań według daty:" << query.lastError().text();
        return tasks;
    }

    while (query.next()) {
        QString task = QString("%1|%2|%3|%4")
                           .arg(query.value("title").toString())
                           .arg(query.value("dueDate").toString())
                           .arg(query.value("priority").toInt())
                           .arg(query.value("description").toString());
        tasks.append(task);
    }

    return tasks;
}

QList<QString> DatabaseManager::getTasksForDate(int userId, const QDate &date) {
    QList<QString> tasks;
    if (!db.isOpen()) return tasks;

    QSqlQuery query(db);
    query.prepare("SELECT title, dueDate, priority, description FROM tasks "
                  "WHERE user_id = :user_id AND DATE(dueDate) = :dueDate");
    query.bindValue(":user_id", userId);
    query.bindValue(":dueDate", date.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        qWarning() << "Błąd SELECT getTasksForDate:" << query.lastError().text();
        return tasks;
    }

    while (query.next()) {
        QString task = QString("%1|%2|%3|%4")
                           .arg(query.value("title").toString())
                           .arg(query.value("dueDate").toString())
                           .arg(query.value("priority").toInt())
                           .arg(query.value("description").toString());
        tasks.append(task);
    }

    return tasks;

}
