#pragma once

#include "models.h"
#include <vector>
#include <string>

struct AppData {
    std::vector<Note> notes;
    std::vector<Task> tasks;
    int nextId = 1;
};

AppData loadData(const std::string& path);
bool saveData(const std::string& path, const AppData& data);