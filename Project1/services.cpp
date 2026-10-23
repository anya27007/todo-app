
#include "services.h"

#include <algorithm>
#include <ctime>
#include <limits>

// Добавление заметки
bool addNote(
    AppData& data,
    const std::string& title,
    const std::string& text,
    Note& created,
    std::string& error
) {
    if (title.empty() || text.empty()) {
        error = "Название и текст заметки не должны быть пустыми.";
        return false;
    }

    if (data.nextId <= 0 ||
        data.nextId == std::numeric_limits<int>::max()) {
        error = "Свободные ID закончились.";
        return false;
    }

    Note note;
    note.id = data.nextId;
    note.title = title;
    note.text = text;
    note.createdAt = std::time(nullptr);

    data.notes.push_back(note);
    data.nextId++;

    created = note;
    error.clear();

    return true;
}

// Добавление задачи
bool addTask(
    AppData& data,
    const std::string& text,
    Task& created,
    std::string& error
) {
    if (text.empty()) {
        error = "Текст задачи не должен быть пустым.";
        return false;
    }

    if (data.nextId <= 0 ||
        data.nextId == std::numeric_limits<int>::max()) {
        error = "Свободные ID закончились.";
        return false;
    }

    Task task;
    task.id = data.nextId;
    task.text = text;
    task.createdAt = std::time(nullptr);
    task.completed = false;

    data.tasks.push_back(task);
    data.nextId++;

    created = task;
    error.clear();

    return true;
}

// Поиск заметки по ID
bool findNoteById(
    const AppData& data,
    int id,
    Note& found
) {
    for (const Note& note : data.notes) {
        if (note.id == id) {
            found = note;
            return true;
        }
    }

    return false;
}

// Поиск задачи по ID
bool findTaskById(
    const AppData& data,
    int id,
    Task& found
) {
    for (const Task& task : data.tasks) {
        if (task.id == id) {
            found = task;
            return true;
        }
    }

    return false;
}

// Удаление заметки или задачи
bool removeById(
    AppData& data,
    int id,
    RecordType& type,
    Note& deletedNote,
    Task& deletedTask,
    std::string& error
) {
    // Сначала ищем заметку
    auto noteIt = std::find_if(
        data.notes.begin(),
        data.notes.end(),
        [id](const Note& note) {
            return note.id == id;
        }
    );

    if (noteIt != data.notes.end()) {
        deletedNote = *noteIt;
        type = RecordType::Note;

        data.notes.erase(noteIt);
        error.clear();

        return true;
    }

    // Затем ищем задачу
    auto taskIt = std::find_if(
        data.tasks.begin(),
        data.tasks.end(),
        [id](const Task& task) {
            return task.id == id;
        }
    );

    if (taskIt != data.tasks.end()) {
        deletedTask = *taskIt;
        type = RecordType::Task;

        data.tasks.erase(taskIt);
        error.clear();

        return true;
    }

    error = "Запись с таким ID не найдена.";
    return false;
}

// Отметка задачи выполненной
CompleteResult markCompleted(
    AppData& data,
    int id
) {
    // Сначала проверяем заметки
    for (const Note& note : data.notes) {
        if (note.id == id) {
            return CompleteResult::IsNote;
        }
    }

    // Затем ищем задачу
    for (Task& task : data.tasks) {
        if (task.id == id) {
            if (task.completed) {
                return CompleteResult::AlreadyCompleted;
            }

            task.completed = true;
            return CompleteResult::Marked;
        }
    }

    return CompleteResult::NotFound;
}

// Получение списка заметок
const std::vector<Note>& getNotes(const AppData& data) {
    return data.notes;
}

// Получение списка задач
const std::vector<Task>& getTasks(const AppData& data) {
    return data.tasks;
}
