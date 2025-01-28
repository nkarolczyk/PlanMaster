#include "mainwindow.h"
#include "databasemanager.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QSplashScreen>
#include <QPixmap>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QVBoxLayout>
#include <QLabel>
#include <QLayoutItem>


Q_DECLARE_METATYPE(Task)

void MainWindow::updateAnalytics() {
    int completedCount = 0;
    int ongoingCount = 0;

    auto tasks = taskManager->getTasks();
    for (const auto &task : tasks) {
        if (task.getIsCompleted()) {
            ++completedCount;
        } else {
            ++ongoingCount;
        }
    }

    //sprawdzenie czy mamy dane do wyświetlenia
    if (completedCount == 0 && ongoingCount == 0) {
        ui->labelAnalytics->setText("Brak danych do wyświetlenia.");
        return;
    } else {
        ui->labelAnalytics->setText("Proporcja ukończonych i nieukończonych zadań");
    }

    //usunięcie poprzednich widgetów (aby nie nakładały się)
    QLayoutItem *child;
    while ((child = ui->widgetChartContainer->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    //NIE DZIALA NIC !!!

    QPieSeries *series = new QPieSeries();
    series->append("Ukończone", completedCount);
    series->append("W toku", ongoingCount);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Proporcja ukończonych i nieukończonych zadań");

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(chartView);
    ui->widgetChartContainer->setLayout(layout);
}

void MainWindow::on_btnCompleteTask_clicked() {
    QListWidgetItem *selectedItem = ui->taskList->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "Błąd", "Wybierz zadanie do oznaczenia jako zakończone.");
        return;
    }

    QVariant var = selectedItem->data(Qt::UserRole);
    if (!var.isValid()) {
        QMessageBox::warning(this, "Błąd", "Nie udało się pobrać danych zadania.");
        return;
    }

    Task task = var.value<Task>();
    task.setIsCompleted(true);
    taskManager->updateTask(task);

    updateTaskList();
    QMessageBox::information(this, "Sukces", "Zadanie oznaczone jako zakończone.");
}


MainWindow::MainWindow(int userId, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), userId(userId) {
    ui->setupUi(this);

    showSplashScreen();
    taskManager = new TaskManager();

    reminderTimer = new QTimer(this);
    connect(reminderTimer, &QTimer::timeout, this, &MainWindow::checkDeadlines);
    reminderTimer->start(60000); // 60000 ms = 1 minuta


    ui->dateInput->setDateTime(QDateTime::currentDateTime());

    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddTaskClicked);
    connect(ui->taskList, &QListWidget::itemClicked, this, &MainWindow::onTaskItemClicked);
    connect(ui->sortDateButton, &QPushButton::clicked, this, &MainWindow::onSortDateClicked);
    connect(ui->sortPriorityButton, &QPushButton::clicked, this, &MainWindow::onSortPriorityClicked);
    connect(ui->btnExportToTxt, &QPushButton::clicked, this, &MainWindow::onExportTasksClicked);
    connect(ui->btnRemoveTask, &QPushButton::clicked, this, &MainWindow::onRemoveTaskClicked);
    connect(ui->btnCompleteTask, &QPushButton::clicked, this, &MainWindow::onCompleteTaskClicked);
    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &MainWindow::onDateSelected);
    connect(ui->btnCompleteTask, &QPushButton::clicked, this, &MainWindow::onCompleteTaskClicked);
    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &MainWindow::onCalendarDateForAdding);


    updateTaskList();
}

MainWindow::~MainWindow() {
    delete taskManager;
    delete ui;
}

int MainWindow::findNextFreeRow(int column) {
    for (int row = 0; row < ui->tableWidgetWeek->rowCount(); ++row) {
        if (!ui->tableWidgetWeek->item(row, column)) {
            return row;
        }
    }

    ui->tableWidgetWeek->insertRow(ui->tableWidgetWeek->rowCount());
    return ui->tableWidgetWeek->rowCount() - 1;
}


void MainWindow::showSplashScreen() {
    QSplashScreen *splash = new QSplashScreen(QPixmap(":/images/splash.png"));
    splash->show();
    QTimer::singleShot(2000, splash, &QSplashScreen::close);
}

void MainWindow::checkDeadlines()
{
    auto tasks = dbManager.getTasksForUser(userId);

    QDateTime now = QDateTime::currentDateTime();

    for (const auto &taskString : tasks) {
        QStringList parts = taskString.split("|");
        if (parts.size() < 5) continue;

        bool isCompleted = (parts[4] == "1");
        if (!isCompleted) {
            QDateTime deadline = QDateTime::fromString(parts[1], Qt::ISODate);
            // 1h przed terminem
            if (deadline.addSecs(-3600) <= now && now < deadline) {
                QMessageBox::information(this, "Przypomnienie",
                                         QString("Zbliża się termin zadania: %1").arg(parts[0]));
            }
        }
    }
}


void MainWindow::displayWeeklyTasks(const QDate &startOfWeek)
{
    // W layoutcie mamy QTableWidget *tableWidgetWeek
    // 7 kolumn – poniedziałek do niedzieli
    // np. w constructorze: tableWidgetWeek->setColumnCount(7);

    // Wyczyść aktualną tabelę
    //tableWidgetWeek->clearContents();

    auto tasks = taskManager->getTasks();

    for (const auto& task : tasks) {
        QDate d = QDateTime::fromSecsSinceEpoch(task.getDueDate()).date();

        if (d >= startOfWeek && d < startOfWeek.addDays(7)) {
            int dayIndex = d.dayOfWeek() - 1; // Monday=1 -> index 0
            int row = findNextFreeRow(dayIndex);
            QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(task.getTitle()));


            ui->tableWidgetWeek->setItem(row, dayIndex, item);

        }
    }
}

void MainWindow::onTaskItemClicked(QListWidgetItem *item) {
    QVariant var = item->data(Qt::UserRole);
    if (!var.isValid()) {
        QMessageBox::warning(this, "Błąd", "Nie udało się pobrać szczegółów zadania.");
        return;
    }

    QString taskString = var.toString();
    QStringList parts = taskString.split("|");

    if (parts.size() < 3) {
        QMessageBox::warning(this, "Błąd", "Niepoprawny format danych zadania.");
        return;
    }

    QString title = parts[0];
    QString dueDate = QDateTime::fromString(parts[1], "yyyy-MM-dd HH:mm:ss").toString("yyyy-MM-dd HH:mm");
    QString priority = QString::number(parts[2].toInt());
    QString description = (parts.size() > 3) ? parts[3] : "Brak opisu";

    QString details = QString("Tytuł: %1\nTermin: %2\nPriorytet: %3\nOpis: %4")
                          .arg(title, dueDate, priority, description);

    QMessageBox::information(this, "Szczegóły zadania", details);
}

void MainWindow::onAddTaskClicked() {
    QString title = ui->titleInput->text().trimmed();
    QString description = ui->descriptionInput->toPlainText().trimmed();
    QDateTime dueDate = ui->dateInput->dateTime();
    int priority = ui->priorityInput->value();

    if (title.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Tytuł zadania nie może być pusty.");
        return;
    }

    bool success = dbManager.addTask(userId, title, dueDate, priority, description, false);
    if (success) {
        QMessageBox::information(this, "Sukces", "Zadanie zostało dodane.");
        updateTaskList();
    } else {
        QMessageBox::warning(this, "Błąd", "Nie udało się dodać zadania.");
    }

    ui->titleInput->clear();
    ui->descriptionInput->clear();
    ui->dateInput->setDateTime(QDateTime::currentDateTime());
    ui->priorityInput->setValue(1);
}

void MainWindow::onRemoveTaskClicked() {
    QListWidgetItem *selectedItem = ui->taskList->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "Błąd", "Wybierz zadanie do usunięcia.");
        return;
    }

    QString taskTitle = selectedItem->text().split(" (").first();

    if (dbManager.removeTask(taskTitle)) {
        delete selectedItem;
        QMessageBox::information(this, "Sukces", "Zadanie zostało usunięte.");
    } else {
        QMessageBox::warning(this, "Błąd", "Nie udało się usunąć zadania.");
    }

    updateTaskList();
    updateAnalytics();
}

void MainWindow::onCompleteTaskClicked() {
    QListWidgetItem *selectedItem = ui->taskList->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "Błąd", "Wybierz zadanie do oznaczenia jako zakończone.");
        return;
    }

    QString taskTitle = selectedItem->text().split(" (").first();

    if (dbManager.markTaskAsCompleted(taskTitle)) {
        QMessageBox::information(this, "Sukces", "Zadanie oznaczone jako ukończone.");
    } else {
        QMessageBox::warning(this, "Błąd", "Nie udało się oznaczyć zadania jako ukończone.");
    }

    updateTaskList();
    updateAnalytics(); //nie działa
}


void MainWindow::onSortDateClicked() {
    ui->taskList->clear();

    QList<QString> tasks = dbManager.getTasksSortedByDate(userId);

    if (tasks.isEmpty()) {
        ui->taskList->addItem("Brak zadań do wyświetlenia.");
        return;
    }

    for (const QString &task : tasks) {
        QStringList parts = task.split("|");
        if (parts.size() >= 4) {
            QString title = parts[0];
            QString dueDate = QDateTime::fromString(parts[1], "yyyy-MM-dd HH:mm:ss").toString("yyyy-MM-dd HH:mm");
            QString priority = QString::number(parts[2].toInt());
            QString description = parts[3];

            QString displayText = QString("%1 (Termin: %2, Priorytet: %3)").arg(title, dueDate, priority);
            QListWidgetItem *item = new QListWidgetItem(displayText);
            item->setData(Qt::UserRole, task);
            ui->taskList->addItem(item);
        }
    }
}


void MainWindow::onSortPriorityClicked() {
    ui->taskList->clear();

    QList<QString> tasks = dbManager.getTasksSortedByPriority(userId);
    qDebug() << "Zadania posortowane według priorytetu:" << tasks;

    if (tasks.isEmpty()) {
        ui->taskList->addItem("Brak zadań do wyświetlenia.");
        return;
    }

    for (const QString &task : tasks) {
        QStringList parts = task.split("|");
        if (parts.size() >= 4) {
            QString title = parts[0];
            QString dueDate = QDateTime::fromString(parts[1], "yyyy-MM-dd HH:mm:ss").toString("yyyy-MM-dd HH:mm");
            QString priority = QString::number(parts[2].toInt());
            QString description = parts[3];

            QString displayText = QString("%1 (Priorytet: %2, Termin: %3)").arg(title, priority, dueDate);
            QListWidgetItem *item = new QListWidgetItem(displayText);
            item->setData(Qt::UserRole, task);
            ui->taskList->addItem(item);
        }
    }
}


void MainWindow::onExportTasksClicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Zapisz jako", "", "Pliki tekstowe (*.txt)");
    if (fileName.isEmpty()) return;

    if (dbManager.exportToTextFile(userId, fileName)) {
        QMessageBox::information(this, "Sukces", "Zadania zostały wyeksportowane.");
    } else {
        QMessageBox::warning(this, "Błąd", "Nie udało się wyeksportować zadań.");
    }

}

void MainWindow::updateTaskList() {
    ui->taskList->clear();

    QList<QString> tasks = dbManager.getTasksForUser(userId);

    if (tasks.isEmpty()) {
        ui->taskList->addItem("Brak zadań w bazie.");
        return;
    }

    for (const QString &taskString : tasks) {
        QStringList parts = taskString.split("|");
        if (parts.size() >= 4) {
            QString title = parts[0];
            QString dueDate = QDateTime::fromString(parts[1], "yyyy-MM-dd HH:mm:ss").toString("yyyy-MM-dd HH:mm");
            QString priority = parts[2];
            QString description = parts[3];

            QString displayText = QString("%1 (Termin: %2, Priorytet: %3)").arg(title, dueDate, priority);
            QListWidgetItem *item = new QListWidgetItem(displayText);
            item->setData(Qt::UserRole, taskString);
            ui->taskList->addItem(item);
        } else {
            qWarning() << "Niepoprawny format danych zadania:" << taskString;
        }
    }
}



void MainWindow::onTaskClicked(QListWidgetItem *item) {
   /* QString taskDisplay = item->text();

    // Rozdzielenie danych przy użyciu separatora "|"
    QList<QString> tasks = dbManager.getTasksForUser(userId);
    for (const QString &taskDescription : tasks) {
        QStringList parts = taskDescription.split("|");
        if (parts.size() == 4 && parts[0] == taskDisplay.split(" - ").first().trimmed()) {
            QString title = parts[0];
            QString dueDate = QDateTime::fromString(parts[1], Qt::ISODate).toString("dd.MM.yyyy HH:mm");
            QString priority = parts[2];
            QString description = parts[3];

            // Wyświetlenie szczegółów w QMessageBox
            QString message = QString("Tytuł: %1\nPriorytet: %2\nTermin: %3\nOpis: %4")
                                  .arg(title)
                                  .arg(priority)
                                  .arg(dueDate)
                                  .arg(description.isEmpty() ? "Brak opisu" : description);
            QMessageBox::information(this, "Szczegóły zadania", message);
            return;
        }
    }

    QMessageBox::warning(this, "Błąd", "Nie znaleziono szczegółów zadania.");
*/
}


void MainWindow::checkReminders() {
    // Funkcja przypomnień do zaimplementowania w przyszłości
}

void MainWindow::onDateSelected(const QDate &date) {
    ui->taskList->clear(); // Wyczyść listę zadań

    QList<QString> tasks = dbManager.getTasksForDate(userId, date);
    QList<QString> tasksSorted = dbManager.getTasksSortedByDate(userId);



    if (tasks.isEmpty()) {
        ui->taskList->addItem("Brak zadań dla wybranej daty.");
        return;
    }

    for (const QString &task : tasks) {
        QStringList parts = task.split("|");
        if (parts.size() >= 4) {
            QString title = parts[0];
            QString dueDate = QDateTime::fromString(parts[1], "yyyy-MM-dd HH:mm:ss").toString("yyyy-MM-dd HH:mm");
            QString priority = QString::number(parts[2].toInt());
            QString description = parts[3];

            QString displayText = QString("%1 (Priorytet: %2, Termin: %3)").arg(title, priority, dueDate);
            ui->taskList->addItem(displayText);
        }
    }
}


void MainWindow::on_btnAbout_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->about);
}



void MainWindow::on_btnHome_clicked()
{
     ui->stackedWidget->setCurrentWidget(ui->home);
    // Odłącz wszystkie poprzednie sygnały kalendarza
    disconnect(ui->calendarWidget, nullptr, nullptr, nullptr);

    // Podłącz funkcję obsługującą wybór daty dla dodawania zadań
    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &MainWindow::onCalendarDateForAdding);

}


void MainWindow::on_btnAnalytics_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->analitics);
    updateAnalytics();
}





void MainWindow::on_btnCalendar_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->calendar);
    disconnect(ui->calendarWidget, nullptr, nullptr, nullptr);
    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &MainWindow::onDateSelected);

}


void MainWindow::on_btnPlanMaster_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->home);
}


void MainWindow::on_btnPlanMaster2_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->home);
}


void MainWindow::onCalendarDateForAdding(const QDate &date) {
    ui->dateInput->setDate(date);
    ui->titleInput->setFocus();
}

void MainWindow::syncTasksWithDatabase() {
    QList<QString> tasks = dbManager.getTasksForUser(userId);
    taskManager->clearTasks();

    for (const QString &taskString : tasks) {
        QStringList parts = taskString.split("|");
        if (parts.size() >= 5) {
            Task task(
                parts[0].toStdString(),
                parts[3].toStdString(),
                parts[2].toInt(),
                QDateTime::fromString(parts[1], Qt::ISODate).toSecsSinceEpoch()
                );
            task.setIsCompleted(parts[4] == "1");
            taskManager->addTask(task);
        }
    }

    qDebug() << "Zsynchronizowano zadania z bazą danych.";
}

