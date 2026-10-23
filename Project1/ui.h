
#pragma once

#include "services.h"

#include <string>


// Чтение строки. false означает конец ввода.
bool readLine(const std::string& prompt, std::string& result);

// Чтение целого числа.
bool readInt(const std::string& prompt, int& result);

// Преобразование времени в строку.
std::string formatDate(std::time_t timestamp);

// Вывод записей.
void printNote(const Note& note);
void printTask(const Task& task);

// Вопрос с ответом да/нет.
// false означает конец ввода, а не ответ "нет".
bool askYesNo(const std::string& prompt, bool& answer);

// Сценарии меню.
bool uiAdd(AppData& data);
bool uiDelete(AppData& data);
bool uiComplete(AppData& data);
bool uiShow(const AppData& data);
