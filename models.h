#pragma once

#include <string>
#include <ctime>
using namespace std;

struct Note {
    int id = 0;
    string title;
    string text;
    time_t createdAt = 0;
};

struct Task {
    int id = 0;
    string text;
    time_t createdAt = 0;
    bool completed = false;
};