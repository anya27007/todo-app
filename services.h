
#pragma once

#include "storage.h"

#include <string>
#include <vector>

// Возможные результаты отметки задачи
enum class CompleteResult {
    NotFound,        // ID не найден
    IsNote,          // ID принадлежит заметке
    Marked,          // Задача отмечена выполненной
    AlreadyCompleted // Задача уже выполнена
};

// Тип удалённой записи
enum class RecordType {
    Note,
    Task
};

// Добавление заметки
bool addNote(
    AppData& data,
    const std::string& title,
    const std::string& text,
    Note& created,
    std::string& error
);

// Добавление задачи
bool addTask(
    AppData& data,
    const std::string& text,
    Task& created,
    std::string& error
);

// Поиск заметки по ID
bool findNoteById(
    const AppData& data,
    int id,
    Note& found
);

// Поиск задачи по ID
bool findTaskById(
    const AppData& data,
    int id,
    Task& found
);

// Удаление заметки или задачи по ID
bool removeById(
    AppData& data,
    int id,
    RecordType& type,
    Note& deletedNote,
    Task& deletedTask,
    std::string& error
);

// Отметка задачи выполненной
CompleteResult markCompleted(
    AppData& data,
    int id
);

// Получение всех заметок и задач
const std::vector<Note>& getNotes(const AppData& data);
const std::vector<Task>& getTasks(const AppData& data);
