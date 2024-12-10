#ifndef TASK_H
#define TASK_H

#include <string>
#include <ctime>
#include <QJsonObject>

class Task {
private:
    std::string title;
    std::string description;
    int priority;
    std::time_t dueDate;
    bool completed;

public:
    Task(const std::string &title, const std::string &description, int priority, std::time_t dueDate);

    std::string getTitle() const;
    std::string getDescription() const;
    int getPriority() const;
    std::time_t getDueDate() const;
    bool isCompleted() const;

    void setTitle(const std::string &newTitle);
    void setDescription(const std::string &newDescription);
    void setPriority(int newPriority);
    void setDueDate(std::time_t newDueDate);
    void markAsCompleted();

    QJsonObject toJson() const;
    static Task fromJson(const QJsonObject &json);
};

#endif // TASK_H
