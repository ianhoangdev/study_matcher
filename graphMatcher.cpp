#include "graphMatcher.h"


// Graph Matching class
GraphMatcher::GraphMatcher() {
    edges.reserve(100000);
    courseMap.reserve(1000);
    envMap.reserve(1000);
    groupMap.reserve(1000);
    styleMap.reserve(1000);
    students.reserve(100000);
}

void GraphMatcher::addStudent(const Student& s) {
    unordered_map<int,int> weightAcc;
    
    for (auto& c : s.courses)
    for (int o : courseMap[c])
        weightAcc[o] += courseWeight;

    for (int o : envMap[s.prefs.environment])
        weightAcc[o] += prefWeight;
    for (int o : groupMap[s.prefs.group_size])
        weightAcc[o] += prefWeight;
    for (int o : styleMap[s.prefs.study_style])
        weightAcc[o] += prefWeight;

    for (auto& [o, w] : weightAcc) {
        if (w > 0) {
        edges[s.ufid].emplace_back(o, w);
        edges[o].emplace_back(s.ufid, w);
        }
    }

    for (auto& c : s.courses)  courseMap[c].push_back(s.ufid);
    envMap[s.prefs.environment].push_back(s.ufid);
    groupMap[s.prefs.group_size].push_back(s.ufid);
    styleMap[s.prefs.study_style].push_back(s.ufid);
    
    students[s.ufid] = s;
}

std::vector<std::pair<int, int>> GraphMatcher::findTopMatches(int ufid, int topN) {
    auto it = edges.find(ufid);
    if (it == edges.end()) return {};
    auto result = it->second;                      
    sort(result.begin(), result.end(),
            [](auto &a, auto &b){ return a.second > b.second; });
    if (result.size() > topN) result.resize(topN);
    return result;
}
