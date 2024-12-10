// DatabaseManager.h
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QCryptographicHash>

class DatabaseManager {
public:
    DatabaseManager();
    bool addTask(int id, const QString &name, const QDateTime &dueDate, int priority);

    bool openDatabase();
    void closeDatabase();
    int getUserId(const QString &username, const QString &password);
    QString hashPassword(const QString &password);

    bool addUser(const QString &username, const QString &password);
    bool authenticate(const QString &username, const QString &password);
    QList<QString> getTasksForUser(int userId);
    bool exportToTextFile(int userId, const QString &fileName);
    bool removeTask(const QString &title);
    bool markTaskAsCompleted(const QString &title);
    QList<QString> getTasksSortedByPriority(int userId);

    bool sortTasksByDueDate(int userId);
    bool sortTasksByPriority(int userId);
    QList<QString> getTasksSortedByDate(int userId);
private:
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
