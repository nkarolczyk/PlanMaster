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
    return QString(hashedPassword.toHex()); //konwertowanie hashu na tekst w systemie szesnastkowym ALE JEST BŁĄD
}

bool DatabaseManager::addTask(int id, const QString& name, const QDateTime& dueDate, int priority) {
    QSqlQuery query;

    query.prepare("INSERT INTO tasks (id, name, due_date, priority) VALUES (:id, :name, :due_date, :priority)");

    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.bindValue(":due_date", dueDate);
    query.bindValue(":priority", priority);

    if (query.exec()) {
        qDebug() << "Task added successfully!";
        return true;
    } else {
        qDebug() << "Error adding task: " << query.lastError().text();
        return false;
    }
}

DatabaseManager::DatabaseManager() {
    if (QSqlDatabase::contains("unique_connection_name")) {
        db = QSqlDatabase::database("unique_connection_name");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", "unique_connection_name");
    }
}

void DatabaseManager::closeDatabase() {
    if (db.isOpen()) {
        db.close();
    }
    QSqlDatabase::removeDatabase(db.connectionName());
}

bool DatabaseManager::openDatabase() {
    if (!db.open()) {
        qWarning() << "Unable to open database:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "username TEXT UNIQUE,"
               "password TEXT)");
    if (query.lastError().isValid()) {
        qWarning() << "Failed to create users table:" << query.lastError().text();
        return false;
    }

    query.exec("CREATE TABLE IF NOT EXISTS tasks ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "user_id INTEGER,"
               "title TEXT,"
               "due_date TEXT,"
               "priority INTEGER,"
               "completed INTEGER DEFAULT 0)");
    if (query.lastError().isValid()) {
        qWarning() << "Failed to create tasks table:" << query.lastError().text();
        return false;
    }

    return true;
}

int DatabaseManager::getUserId(const QString &username, const QString &password) {
    if (!db.isOpen()) return -1;

    QSqlQuery query(db);
    query.prepare("SELECT id, password FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        QString storedPassword = query.value(1).toString();
        if (storedPassword == hashPassword(password)) {
            return query.value(0).toInt();  //id użytkownika
        } else {
            qWarning() << "Password mismatch!";
        }
    } else {
        qWarning() << "Failed to retrieve user ID:" << query.lastError().text();
    }

    return -1;
}


bool DatabaseManager::addUser(const QString &username, const QString &password) {
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next() && query.value(0).toInt() > 0) {
        qWarning() << "User already exists.";
        QMessageBox::warning(nullptr, "Błąd rejestracji", "Użytkownik o tej nazwie już istnieje.");
        return false;
    }

    QString hashedPassword = hashPassword(password);

    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", hashedPassword);

    if (query.exec()) {
        return true;
    } else {
        qWarning() << "Failed to add user:" << query.lastError().text();
        QMessageBox::warning(nullptr, "Błąd rejestracji", "Rejestracja nie powiodła się. Sprawdź logi.");
        return false;
    }
}

bool DatabaseManager::authenticate(const QString &username, const QString &password) {
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("SELECT id, password FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec()) {
        if (query.next()) {
            QString storedPassword = query.value(1).toString();
            if (storedPassword == hashPassword(password)) {
                return true;
            }
        } else {
            qDebug() << "No matching user found!";
        }
    } else {
        qWarning() << "Authentication failed:" << query.lastError().text();
    }

    return false;
}

QList<QString> DatabaseManager::getTasksForUser(int userId) {
    QList<QString> tasks;
    if (!db.isOpen()) return tasks;

    QSqlQuery query(db);
    query.prepare("SELECT title, due_date, priority FROM tasks WHERE user_id = :user_id ORDER BY due_date");
    query.bindValue(":user_id", userId);

    if (query.exec()) {
        while (query.next()) {
            QString task = QString("[Priority: %1] %2 - Due: %3")
                               .arg(query.value(2).toInt())
                               .arg(query.value(0).toString())
                               .arg(query.value(1).toString());
            tasks.append(task);
        }
    } else {
        qWarning() << "Failed to retrieve tasks:" << query.lastError().text();
    }

    return tasks;
}

bool DatabaseManager::removeTask(const QString &title) {
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("DELETE FROM tasks WHERE title = :title");
    query.bindValue(":title", title);

    if (!query.exec()) {
        qWarning() << "Failed to remove task:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::markTaskAsCompleted(const QString &title) {
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("UPDATE tasks SET completed = 1 WHERE title = :title");
    query.bindValue(":title", title);

    if (!query.exec()) {
        qWarning() << "Failed to mark task as completed:" << query.lastError().text();
        return false;
    }

    return true;
}
QList<QString> DatabaseManager::getTasksSortedByPriority(int userId) {
    QList<QString> tasks;
    if (!db.isOpen()) return tasks;

    QSqlQuery query(db);
    query.prepare("SELECT title, due_date, priority FROM tasks WHERE user_id = :user_id ORDER BY priority DESC");
    query.bindValue(":user_id", userId);

    if (query.exec()) {
        while (query.next()) {
            QString task = QString("[Priority: %1] %2 - Due: %3")
                               .arg(query.value(2).toInt())
                               .arg(query.value(0).toString())
                               .arg(query.value(1).toString());
            tasks.append(task);
        }
    } else {
        qWarning() << "Failed to retrieve tasks:" << query.lastError().text();
    }

    return tasks;
}


bool DatabaseManager::exportToTextFile(int userId, const QString &fileName) {
    if (!db.isOpen()) return false;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for export:" << file.errorString();
        return false;
    }

    QTextStream out(&file);

    QSqlQuery query(db);
    query.prepare("SELECT title, due_date, priority FROM tasks WHERE user_id = :user_id");
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
