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
        qWarning() << "Failed to retrieve tasks by priority:" << query.lastError().text();
    }

    return tasks;
}

bool DatabaseManager::openDatabase() {
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
    }
}

bool DatabaseManager::addTask(int userId, const QString &name, const QDateTime &dueDate, int priority) {
    if (!db.isOpen()) {
        qWarning() << "Baza danych nie jest otwarta.";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO Tasks (userId, title, dueDate, priority) VALUES (:userId, :title, :dueDate, :priority)");
    query.bindValue(":userId", userId);
    query.bindValue(":title", name);
    query.bindValue(":dueDate", dueDate.toString(Qt::ISODate));
    query.bindValue(":priority", priority);

    qDebug() << "Wartości dodawane do bazy danych:";
    qDebug() << "userId:" << userId;
    qDebug() << "title:" << name;
    qDebug() << "dueDate:" << dueDate.toString(Qt::ISODate);
    qDebug() << "priority:" << priority;

    if (!query.exec()) {
        qWarning() << "Błąd dodawania zadania:" << query.lastError().text();
        return false;
    }

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

bool DatabaseManager::markTaskAsCompleted(const QString &title) {
    if (!openDatabase()) return false;

    QSqlQuery query(db);
    query.prepare("UPDATE tasks SET completed = 1 WHERE title = :title");
    query.bindValue(":title", title);

    if (!query.exec()) {
        qWarning() << "Failed to mark task as completed:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::authenticate(const QString &username, const QString &password) {
    if (!openDatabase()) return false;

    QSqlQuery query(db);
    query.prepare("SELECT password FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        QString storedPassword = query.value(0).toString();
        return storedPassword == hashPassword(password);
    } else {
        qWarning() << "Authentication failed:" << query.lastError().text();
        return false;
    }
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

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qWarning() << "Query execution failed:" << query.lastError().text();
        return false;
    }

    if (query.next() && query.value(0).toInt() > 0) {
        QMessageBox::warning(nullptr, "Błąd rejestracji", "Użytkownik o tej nazwie już istnieje.");
        return false;
    }

    QString hashedPassword = hashPassword(password);

    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", hashedPassword);

    if (!query.exec()) {
        qWarning() << "Failed to add user:" << query.lastError().text();
        QMessageBox::warning(nullptr, "Błąd rejestracji", "Rejestracja nie powiodła się.");
        return false;
    }

    return true;
}

QList<QString> DatabaseManager::getTasksForUser(int userId) {
    QList<QString> tasks;

    if (!db.isOpen()) {
        qWarning() << "Baza danych nie jest otwarta.";
        return tasks;
    }

    QSqlQuery query(db);
    query.prepare("SELECT title, dueDate, priority FROM Tasks WHERE userId = :userId ORDER BY dueDate");
    query.bindValue(":userId", userId);

    if (query.exec()) {
        while (query.next()) {
            QString task = QString("[Priorytet: %1] %2 - Termin: %3")
                               .arg(query.value(2).toInt())
                               .arg(query.value(0).toString())
                               .arg(query.value(1).toString());
            tasks.append(task);
        }
    } else {
        qWarning() << "Błąd pobierania zadań:" << query.lastError().text();
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
