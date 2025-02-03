# PlanMaster  

Ostateczna zmiana Readme: 03.02.2025r  

PlanMaster to nowoczesna aplikacja do zarządzania zadaniami i planowania tygodniowego, pełniąca rolę wirtualnego asystenta. Została stworzona, aby pomóc użytkownikom w organizowaniu obowiązków, przypominaniu o terminach oraz umożliwieniu efektywnego planowania. Dzięki intuicyjnemu interfejsowi graficznemu w Qt, użytkownicy mogą dodawać, edytować i sortować zadania według priorytetów oraz monitorować pozostały czas do ich realizacji. Program obsługuje automatyczne powiadomienia, zapewniając pełną kontrolę nad organizacją codziennych i tygodniowych zadań.


FUNKCJE:
* Tworzenie, edycja i usuwanie zadań z opcjami priorytetu.
* Planowanie tygodniowe: widok zadań w podziale na dni tygodnia.
* Powiadomienia o nadchodzących terminach i czasie pozostałym.
* Sortowanie zadań według priorytetu lub terminu.
* Przechowywanie danych w bazie SQLite dla zachowania historii i możliwości kontynuacji pracy.
* Eksportowanie zadań do pliku tekstowego.
* Generowanie raportów i statystyk dotyczących zadań.


WYKORZYSTANE TECHNOLOGIE:  
C++ - główny język programowania  
Qt6 (Widgets, Sql, Charts) - framework do budowy interfejsu graficznego i obsługi bazy danych  
SQLite - baza danych do przechowywania zadań i użytkowników  
CMake - system budowania projektu  


STRUKTURA PROJEKTU:  
``` main.cpp ``` - punkt wejścia do aplikacji  
``` mainwindow.h/.cpp ``` - główne okno aplikacji  
``` logindialog.h/.cpp ``` - okno logowania użytkownika  
``` registerdialog.h/.cpp ``` - okno rejestracji użytkownika  
``` databasemanager.h/.cpp ``` - zarządzanie bazą danych SQLite  
``` task.h/.cpp ``` - klasa reprezentująca pojedyncze zadanie  
``` taskmanager.h/.cpp ``` - klasa zarządzająca zadaniami  
``` CMakeLists.txt ``` - konfiguracja budowania projektu  


HARMONOGRAM PROJEKTU (ostateczna wersja)  
Tydzień 1 (do 21.10) – Planowanie i Projektowanie
* Określenie wymagań funkcjonalnych.
* Stworzenie repozytorium na GitHubie.
* Zaprojektowanie struktury projektu i planu plików (np. klasy, GUI, zasoby).

Tydzień 2 (do 28.10) – Tworzenie Podstawowych Klas  
* Implementacja klasy Task przechowującej szczegóły zadań (tytuł, opis, priorytet, data).
* Implementacja klasy TaskManager zarządzającej listą zadań.

Tydzień 3 (do 04.11) – Implementacja GUI – Widok główny  
* Stworzenie głównego okna aplikacji w Qt, wyświetlającego listę zadań.
* Podstawowa funkcjonalność dodawania i edycji zadań przez GUI.
* Połączenie GUI z logiką klas Task i TaskManager.

Tydzień 4 (do 25.11) – Funkcjonalności Sortowania i Widok Tygodniowy  
* Dodanie opcji sortowania zadań według priorytetu i terminu.
* Implementacja widoku tygodniowego w GUI, aby pokazywać zadania w podziale na dni tygodnia.

Tydzień 5 (do 02.12) – Zarządzanie Plikami i Przechowywanie Danych  
* Implementacja zapisu zadań do pliku (.txt).
* Testy poprawności zapisu i odczytu danych użytkownika.

Tydzień 6 (do 09.12) – Implementacja Przypomnień i Powiadomień  
* Stworzenie systemu przypomnień przy użyciu QTimer i sygnałów Qt.
* Wizualne powiadomienia w GUI dla zadań zbliżających się do terminu wykonania.

Tydzień 7 (do 02.12) – Dodanie Statystyk i Raportów  
* Implementacja modułu generowania raportów tygodniowych i statystyk (np. liczba ukończonych zadań).
* Wyświetlanie graficznych analiz w GUI, aby użytkownik mógł ocenić swoją produktywność.

Tydzień 8 (do 09.12) – Optymalizacja GUI i Ulepszenia UX  
* Optymalizacja wyglądu GUI i poprawa UX.
* Ulepszenie obsługi zdarzeń w GUI, takich jak przyciski szybkiego dostępu do najważniejszych funkcji.

Tydzień 9 (do 13.01) – Testowanie i Poprawki  
* Kompleksowe testy działania aplikacji, naprawa błędów.
* Optymalizacja kodu i przegląd funkcji pod kątem wydajności.

Tydzień 10 (do 03.02) – Finalizacja i Dokumentacja  
* Przygotowanie dokumentacji dla użytkowników końcowych i instrukcji instalacji.
* Dokończenie repozytorium GitHub (README, zrzuty ekranu, przykłady użycia).
* Ostateczne testy i przygotowanie aplikacji do prezentacji.

---

INSTALACJA I URUCHOMIENIE   
Wymagania  
* Qt6
* CMake >= 3.16
* Kompilator obsługujący C++17

Kroki instalacji    
Sklonuj repozytorium:
```bash
git clone https://github.com/user/PlanMaster.git
cd PlanMaster
```
Stwórz katalog build i przejdź do niego:
```bash
mkdir build && cd build
```
Skonfiguruj projekt i skompiluj:
```bash
cmake ..
make
```
Uruchom aplikację:
```bash
./PlanMaster
```

