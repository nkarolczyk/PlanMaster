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
#include <QMap>

/**
 * Klasa do zarządzania połączeniem i operacjami na bazie danych (SQLite).
 */
class DatabaseManager {
public:
    DatabaseManager();
    // Baza danych
    bool openDatabase();
    void closeDatabase();

    // Użytkownicy
    bool addUser(const QString &username, const QString &password);
    bool authenticate(const QString &username, const QString &password);
    int getUserId(const QString &username, const QString &password);

    // Zadania
    /**
     * Dodawanie zadania z 6 argumentami:
     * @param userId       - ID użytkownika
     * @param title        - tytuł zadania
     * @param dueDate      - termin (data i czas)
     * @param priority     - priorytet (liczba)
     * @param description  - dodatkowy opis zadania
     * @param completed    - flaga ukończenia (true/false -> 1/0 w bazie)
     */
    bool addTask(int userId,
                 const QString &title,
                 const QDateTime &dueDate,
                 int priority,
                 const QString &description,
                 bool completed);

    /**
     * Usuwanie zadania po tytule.
     * Uwaga: tytuły mogą się powtarzać, więc lepsze jest usuwanie po ID.
     */
    bool removeTask(const QString &title);

    /**
     * Oznaczenie zadania jako ukończone - teraz po ID zamiast po tytule.
     */
    bool markTaskAsCompleted(int userId, const QString &title);



    /**
     * Pobranie listy zadań dla danego użytkownika.
     * Może zwracać dane w formacie "id|title|dueDate|priority|description|completed".
     */
    QList<QString> getTasksForUser(int userId);

    /**
     * Pobranie zadań posortowanych po priorytecie (malejąco).
     */
    QList<QString> getTasksSortedByPriority(int userId);

    /**
     * Eksport zadań do pliku tekstowego.
     */
    bool exportToTextFile(int userId, const QString &fileName);

    /**
     * Pobranie statystyk zadań (np. ukończone / nieukończone).
     */
    QMap<QString, int> getTaskStatistics(int userId);

    /**
     * Pobranie szczegółów jednego zadania po ID (zwraca np. "id|title|dueDate|priority|description|completed").
     */
    QString getTaskById(int taskId);
    QList<QString> getTasksForDate(int userId, const QDate &date);
    QList<QString> getTasksSortedByDate(int userId);

private:
    QSqlDatabase db;

    /**
     * Funkcja do hashowania haseł (SHA256). NIE DZIAŁA JESZCZE
     */
    QString hashPassword(const QString &password);
};

#endif // DATABASEMANAGER_H
