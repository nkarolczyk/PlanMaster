#ifndef TASK_H
#define TASK_H

#include <string>
#include <ctime>

class Task {
private:
    std::string title;
    std::string description;
    int priority;
    std::time_t dueDate;

public:
    Task(const std::string &title, const std::string &description, int priority, std::time_t dueDate);

    std::string getTitle() const;
    std::string getDescription() const;
    int getPriority() const;
    std::time_t getDueDate() const;

    void setTitle(const std::string &newTitle);
    void setDescription(const std::string &newDescription);
    void setPriority(int newPriority);
    void setDueDate(std::time_t newDueDate);
};

#endif
