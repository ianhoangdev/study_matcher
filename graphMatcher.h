#pragma once

#include "types.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>
#include <string>

class GraphMatcher {
public:
    void addStudent(const Student& s);
    std::vector<std::pair<int, int>> findTopMatches(int ufid, int topN = 5);

private:
    std::unordered_map<int, Student> students;
    std::unordered_map<int, std::unordered_map<int, int>> edges;
    std::unordered_map<std::string, std::unordered_set<int>> courseMap;
    const int courseWeight = 10;
    const int prefWeight = 5;

    int countCourseOverlap(const std::set<std::string>& a, const std::set<std::string>& b) const;
    int countPreferenceOverlap(const Preferences& a, const Preferences& b) const;
};
