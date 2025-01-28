#include "task.h"

Task::Task(const std::string &title, const std::string &description, int priority, std::time_t dueDate)
    : title(title), description(description), priority(priority), dueDate(dueDate), isCompleted(false) {}

std::string Task::getTitle() const { return title; }
std::string Task::getDescription() const { return description; }
int Task::getPriority() const { return priority; }
std::time_t Task::getDueDate() const { return dueDate; }
bool Task::getIsCompleted() const { return isCompleted; }

void Task::setTitle(const std::string &newTitle) { title = newTitle; }
void Task::setDescription(const std::string &newDescription) { description = newDescription; }
void Task::setPriority(int newPriority) { priority = newPriority; }
void Task::setDueDate(std::time_t newDueDate) { dueDate = newDueDate; }
void Task::setIsCompleted(bool completed) { isCompleted = completed; }
void Task::markAsCompleted() { isCompleted = true; }
void Task::markAsDone() {
    isDone = true;
}


QJsonObject Task::toJson() const {
    QJsonObject json;
    json["title"] = QString::fromStdString(title);
    json["description"] = QString::fromStdString(description);
    json["priority"] = priority;
    json["dueDate"] = static_cast<qint64>(dueDate);
    json["isCompleted"] = isCompleted;
    return json;
}

Task Task::fromJson(const QJsonObject &json) {
    Task task(
        json["title"].toString().toStdString(),
        json["description"].toString().toStdString(),
        json["priority"].toInt(),
        static_cast<std::time_t>(json["dueDate"].toVariant().toLongLong())
        );
    task.setIsCompleted(json["isCompleted"].toBool());
    return task;
}
