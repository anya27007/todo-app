#include "ui.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <limits>
#include <algorithm>
#include <cctype>

// Чтение строки
bool readLine(const std::string& prompt, std::string& result) {
    std::cout << prompt;

    if (!std::getline(std::cin, result)) {
        return false;
    }

    return true;
}

// Чтение целого числа
bool readInt(const std::string& prompt, int& result) {
    while (true) {
        std::string input;

        if (!readLine(prompt, input)) {
            return false;
        }

        std::istringstream stream(input);
        long long number;
        char extra;

        // Проверяем число и отсутствие лишних символов
        if ((stream >> number) &&
            !(stream >> extra) &&
            number >= std::numeric_limits<int>::min() &&
            number <= std::numeric_limits<int>::max()) {
            result = static_cast<int>(number);
            return true;
        }

        std::cout << "Ошибка: введите целое число.\n";
    }
}

// Преобразование времени в строку
std::string formatDate(std::time_t timestamp) {
    std::tm localTime{};

#ifdef _WIN32
    if (localtime_s(&localTime, &timestamp) != 0) {
        return "Неизвестно";
    }
#else
    if (localtime_r(&timestamp, &localTime) == nullptr) {
        return "Неизвестно";
    }
#endif

    char buffer[32];

    if (std::strftime(
        buffer,
        sizeof(buffer),
        "%d.%m.%Y %H:%M",
        &localTime
    ) == 0) {
        return "Неизвестно";
    }

    return buffer;
}

// Вывод заметки
void printNote(const Note& note) {
    std::cout << "ID: " << note.id << '\n';
    std::cout << "Название: " << note.title << '\n';
    std::cout << "Текст: " << note.text << '\n';
    std::cout << "Дата создания: "
        << formatDate(note.createdAt) << '\n';
}

// Вывод задачи
void printTask(const Task& task) {
    std::cout << "ID: " << task.id << '\n';
    std::cout << "Название: Задача\n";
    std::cout << "Текст: " << task.text << '\n';
    std::cout << "Дата создания: "
        << formatDate(task.createdAt) << '\n';
    std::cout << "Статус: "
        << (task.completed ? "Выполнена" : "Не выполнена")
        << '\n';
}

// Вопрос с ответом да/нет
bool askYesNo(const std::string& prompt, bool& answer) {
    while (true) {
        std::string input;

        if (!readLine(prompt + " да - (y/yes), нет - (n/no): ", input)) {
            return false;
        }

        // Убираем пробелы по краям
        auto first = input.find_first_not_of(" \t");
        if (first == std::string::npos) {
            std::cout << "Введите да(y/yes) или нет(n/no): \n";
            continue;
        }

        auto last = input.find_last_not_of(" \t");
        input = input.substr(first, last - first + 1);

        // Для латинских ответов не важен регистр
        std::transform(
            input.begin(),
            input.end(),
            input.begin(),
            [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            }
        );

        if (input == "y" || input == "yes") {
            answer = true;
            return true;
        }

        if (input == "n" || input == "no") {
            answer = false;
            return true;
        }

        std::cout << "Введите да или нет.\n";
    }
}

// Добавление заметки или задачи
bool uiAdd(AppData& data) {
    std::string type;

    if (!readLine(
        "Что добавить? (1 — заметка, 2 — задача): ",
        type
    )) {
        return false;
    }

    if (type == "1" || type == "n" || type == "N") {
        std::string title;
        std::string text;
        std::string error;
        Note created;

        if (!readLine("Название: ", title) ||
            !readLine("Текст: ", text)) {
            return false;
        }

        if (addNote(data, title, text, created, error)) {
            std::cout << "\nЗаметка создана:\n";
            printNote(created);
        }
        else {
            std::cout << "Ошибка: " << error << '\n';
        }

        return true;
    }

    if (type == "2" || type == "t" || type == "T") {
        std::string text;
        std::string error;
        Task created;

        if (!readLine("Текст задачи: ", text)) {
            return false;
        }

        if (addTask(data, text, created, error)) {
            std::cout << "\nЗадача создана:\n";
            printTask(created);
        }
        else {
            std::cout << "Ошибка: " << error << '\n';
        }

        return true;
    }

    std::cout << "Неизвестный тип записи.\n";
    return true;
}

// Удаление заметки или задачи
bool uiDelete(AppData& data) {
    int id;

    if (!readInt("Введите ID для удаления: ", id)) {
        return false;
    }

    RecordType type{};
    Note deletedNote;
    Task deletedTask;
    std::string error;

    if (!removeById(
        data,
        id,
        type,
        deletedNote,
        deletedTask,
        error
    )) {
        std::cout << error << '\n';
        return true;
    }

    std::cout << "\nУдалена запись:\n";

    if (type == RecordType::Note) {
        printNote(deletedNote);
    }
    else {
        printTask(deletedTask);
    }

    return true;
}

// Отметка задачи выполненной
bool uiComplete(AppData& data) {
    int id;

    if (!readInt("Введите ID задачи: ", id)) {
        return false;
    }

    CompleteResult result = markCompleted(data, id);

    switch (result) {
    case CompleteResult::NotFound:
        std::cout << "Запись с таким ID не найдена.\n";
        return true;

    case CompleteResult::IsNote:
        std::cout << "Это заметка. У неё нет статуса выполнения.\n";
        return true;

    case CompleteResult::AlreadyCompleted: {
        std::cout << "Задача уже выполнена.\n";

        Task task;
        if (findTaskById(data, id, task)) {
            printTask(task);
        }

        break;
    }

    case CompleteResult::Marked: {
        std::cout << "Задача отмечена выполненной.\n";

        Task task;
        if (findTaskById(data, id, task)) {
            printTask(task);
        }

        break;
    }
    }

    bool remove;

    if (!askYesNo("Удалить эту задачу?", remove)) {
        return false;
    }

    if (remove) {
        RecordType type{};
        Note deletedNote;
        Task deletedTask;
        std::string error;

        if (removeById(
            data,
            id,
            type,
            deletedNote,
            deletedTask,
            error
        )) {
            std::cout << "Задача удалена.\n";
        }
        else {
            std::cout << error << '\n';
        }
    }

    return true;
}

// Вывод всех заметок и задач
bool uiShow(const AppData& data) {
    const auto& notes = getNotes(data);
    const auto& tasks = getTasks(data);

    if (notes.empty() && tasks.empty()) {
        std::cout << "Пусто.\n";
        return true;
    }

    std::cout << "\n=== Заметки ===\n";

    if (notes.empty()) {
        std::cout << "Заметок нет.\n";
    }
    else {
        for (const Note& note : notes) {
            std::cout << "\n";
            printNote(note);
            std::cout << "--------------------\n";
        }
    }

    std::cout << "\n=== Задачи ===\n";

    if (tasks.empty()) {
        std::cout << "Задач нет.\n";
    }
    else {
        for (const Task& task : tasks) {
            std::cout << "\n";
            printTask(task);
            std::cout << "--------------------\n";
        }
    }

    return true;
}