#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "task.h"
#include <vector>
#include <algorithm>

class TaskManager {
private:
    std::vector<Task> tasks; //lista zadań

public:
    void addTask(const Task &task);                   //dodaje zadanie
    void removeTask(const std::string &title);        //usuwa zadanie po tytule
    void displayTasks() const;                        //wyświetla wszystkie zadania w konsoli
    void sortByPriority();                            //sortuje zadania według priorytetu
    void sortByDueDate();                             //sortuje zadania według terminu
    const std::vector<Task>& getTasks() const;        //pobiera listę zadań
};

#endif /
