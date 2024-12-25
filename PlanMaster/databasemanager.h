#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QCryptographicHash>
#include <QDebug>

class DatabaseManager {
public:
    DatabaseManager();

    //baza danych
    bool openDatabase();
    void closeDatabase();

    //użytkownicy
    bool addUser(const QString &username, const QString &password);
    bool authenticate(const QString &username, const QString &password);
    int getUserId(const QString &username, const QString &password);

    //zadania
    bool addTask(int userId, const QString &name, const QDateTime &dueDate, int priority);
    bool removeTask(const QString &title);
    bool markTaskAsCompleted(const QString &title);
    QList<QString> getTasksForUser(int userId);
    QList<QString> getTasksSortedByPriority(int userId);
    bool exportToTextFile(int userId, const QString &fileName);

private:
    QSqlDatabase db;
    QString hashPassword(const QString &password);
};

#endif // DATABASEMANAGER_H
