#include "task.h"

Task::Task(const std::string &title, const std::string &description, int priority, std::time_t dueDate)
    : title(title), description(description), priority(priority), dueDate(dueDate) {}

std::string Task::getTitle() const { return title; }
std::string Task::getDescription() const { return description; }
int Task::getPriority() const { return priority; }
std::time_t Task::getDueDate() const { return dueDate; }

void Task::setTitle(const std::string &newTitle) { title = newTitle; }
void Task::setDescription(const std::string &newDescription) { description = newDescription; }
void Task::setPriority(int newPriority) { priority = newPriority; }
void Task::setDueDate(std::time_t newDueDate) { dueDate = newDueDate; }
