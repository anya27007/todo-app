#include "storage.h"
#include "ui.h"

#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

// Имя файла данных задаётся в одном месте
const std::string DATA_FILE = "data.txt";

int main() {
    // Настройка консоли для русского языка

#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    // Загружаем данные при запуске
    AppData data = loadData(DATA_FILE);

    bool running = true;

    while (running) {
        std::cout << "\n=== Мой планировщик ===\n"
                  << "1. Добавить запись\n"
                  << "2. Удалить запись\n"
                  << "3. Отметить задачу выполненной\n"
                  << "4. Показать все записи\n"
                  << "0. Выход\n";

        int choice;

        if (!readInt("Выберите действие: ", choice)) {
            // Конец ввода
            break;
        }

        bool changed = false;
        bool continueRunning = true;

        switch (choice) {
        case 1:
            changed = true;
            continueRunning = uiAdd(data);
            break;

        case 2:
            changed = true;
            continueRunning = uiDelete(data);
            break;

        case 3:
            changed = true;
            continueRunning = uiComplete(data);
            break;

        case 4:
            uiShow(data);
            break;

        case 0:
            running = false;
            continue;
            
        default:
            std::cout << "Нет такого пункта.\n";
            continue;
        }

        // Сохраняем данные после операций изменения
        if (changed) {
            if (!saveData(DATA_FILE, data)) {
                std::cerr << "Ошибка: не удалось сохранить данные!\n";
            }
        }

        if (!continueRunning) {
            running = false;
        }
    }

    return 0;
}

