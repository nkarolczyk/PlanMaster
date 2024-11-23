#ifndef TASK_H
#define TASK_H

#include <string>
#include <ctime>
#include <QJsonObject> // Dodano dla obsługi JSON

class Task {
private:
    std::string title;       // Tytuł zadania
    std::string description; // Opis zadania
    int priority;            // Priorytet zadania
    std::time_t dueDate;     // Termin w formacie czasowym

public:
    // Konstruktor
    Task(const std::string &title, const std::string &description, int priority, std::time_t dueDate);

    // Gettery
    std::string getTitle() const;
    std::string getDescription() const;
    int getPriority() const;
    std::time_t getDueDate() const;

    // Settery
    void setTitle(const std::string &newTitle);
    void setDescription(const std::string &newDescription);
    void setPriority(int newPriority);
    void setDueDate(std::time_t newDueDate);

    // Konwersja do i z obiektu JSON
    QJsonObject toJson() const;                // Eksport do JSON
    static Task fromJson(const QJsonObject &json); // Import z JSON
};

#endif // TASK_H
