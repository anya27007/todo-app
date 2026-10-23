#include "storage.h"

#include <fstream>
#include <algorithm>
#include <limits>

namespace {

    const char DELIMITER = '|';

    // Экранирование текста перед записью в файл
    std::string escapeText(const std::string& text) {
        std::string result;

        for (char c : text) {
            switch (c) {
            case '\\':
                result += "\\\\";
                break;
            case '|':
                result += "\\|";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            default:
                result += c;
            }
        }

        return result;
    }

    // Восстановление исходного текста после чтения
    std::string unescapeText(const std::string& text) {
        std::string result;

        for (std::size_t i = 0; i < text.size(); ++i) {
            if (text[i] == '\\' && i + 1 < text.size()) {
                char next = text[i + 1];

                switch (next) {
                case '\\':
                    result += '\\';
                    ++i;
                    break;
                case '|':
                    result += '|';
                    ++i;
                    break;
                case 'n':
                    result += '\n';
                    ++i;
                    break;
                case 'r':
                    result += '\r';
                    ++i;
                    break;
                default:
                    result += text[i];
                }
            }
            else {
                result += text[i];
            }
        }

        return result;
    }

    // Разделение строки с учётом экранированных разделителей
    std::vector<std::string> splitLine(const std::string& line) {
        std::vector<std::string> fields;
        std::string field;

        for (std::size_t i = 0; i < line.size(); ++i) {
            char c = line[i];

            if (c == '\\' && i + 1 < line.size()) {
                field += c;
                field += line[++i];
            }
            else if (c == DELIMITER) {
                fields.push_back(field);
                field.clear();
            }
            else {
                field += c;
            }
        }

        fields.push_back(field);
        return fields;
    }

    // Безопасное преобразование строки в целое число
    bool parseInteger(const std::string& text, long long& result) {
        if (text.empty()) {
            return false;
        }

        try {
            std::size_t position = 0;
            result = std::stoll(text, &position);

            return position == text.size();
        }
        catch (...) {
            return false;
        }
    }

} // namespace

// Сохранение данных в файл
bool saveData(const std::string& path, const AppData& data) {
    std::ofstream file(path);

    if (!file.is_open()) {
        return false;
    }

    file << data.nextId << '\n';

    for (const Note& note : data.notes) {
        file << "N"
            << DELIMITER << note.id
            << DELIMITER << static_cast<long long>(note.createdAt)
            << DELIMITER << escapeText(note.title)
            << DELIMITER << escapeText(note.text)
            << '\n';
    }

    for (const Task& task : data.tasks) {
        file << "T"
            << DELIMITER << task.id
            << DELIMITER << static_cast<long long>(task.createdAt)
            << DELIMITER << (task.completed ? 1 : 0)
            << DELIMITER << escapeText(task.text)
            << '\n';
    }

    file.flush();

    return file.good();
}

// Загрузка данных из файла
AppData loadData(const std::string& path) {
    AppData data;
    data.nextId = 1;

    std::ifstream file(path);

    // Отсутствие файла не является ошибкой
    if (!file.is_open()) {
        return data;
    }

    std::string line;

    // Первая строка — следующий ID
    if (std::getline(file, line)) {
        long long nextId;

        if (parseInteger(line, nextId) &&
            nextId >= 1 &&
            nextId <= std::numeric_limits<int>::max()) {
            data.nextId = static_cast<int>(nextId);
        }
    }

    int maxId = 0;

    while (std::getline(file, line)) {
        std::vector<std::string> fields = splitLine(line);

        if (fields.empty()) {
            continue;
        }

        // Заметка: N|id|createdAt|title|text
        if (fields[0] == "N" && fields.size() == 5) {
            long long id;
            long long createdAt;

            if (!parseInteger(fields[1], id) ||
                !parseInteger(fields[2], createdAt) ||
                id < 1 ||
                id > std::numeric_limits<int>::max() ||
                createdAt < 0) {
                continue;
            }

            Note note;
            note.id = static_cast<int>(id);
            note.createdAt = static_cast<std::time_t>(createdAt);
            note.title = unescapeText(fields[3]);
            note.text = unescapeText(fields[4]);

            data.notes.push_back(note);
            maxId = std::max(maxId, note.id);
        }

        // Задача: T|id|createdAt|completed|text
        else if (fields[0] == "T" && fields.size() == 5) {
            long long id;
            long long createdAt;
            long long completed;

            if (!parseInteger(fields[1], id) ||
                !parseInteger(fields[2], createdAt) ||
                !parseInteger(fields[3], completed) ||
                id < 1 ||
                id > std::numeric_limits<int>::max() ||
                createdAt < 0 ||
                (completed != 0 && completed != 1)) {
                continue;
            }

            Task task;
            task.id = static_cast<int>(id);
            task.createdAt = static_cast<std::time_t>(createdAt);
            task.completed = (completed == 1);
            task.text = unescapeText(fields[4]);

            data.tasks.push_back(task);
            maxId = std::max(maxId, task.id);
        }

        // Неизвестный тип или повреждённая строка пропускаются
    }

    // Следующий ID должен быть больше всех существующих ID
    if (maxId == std::numeric_limits<int>::max()) {
        data.nextId = maxId;
    }
    else {
        data.nextId = std::max(data.nextId, maxId + 1);
    }

    return data;
}