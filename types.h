#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <set>

struct Preferences {
    std::string environment;
    std::string group_size;
    std::string study_style;
};

struct Student {
    int ufid;
    std::set<std::string> courses;
    Preferences prefs;
};

#endif
