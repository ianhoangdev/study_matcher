#pragma once

#include "types.h"
#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <utility>
using namespace std;

class GraphMatcher {
public:
    void addStudent(const Student& s);
    vector<pair<int, int>> findTopMatches(int ufid, int topN = 5);
    GraphMatcher();

private:
    unordered_map<int, Student> students;
    unordered_map<int, vector<pair<int,int>>> edges;
    unordered_map<string, vector<int>> courseMap;
    unordered_map<string, vector<int>> envMap, groupMap, styleMap;
    const int courseWeight = 10;
    const int prefWeight = 5;
};
