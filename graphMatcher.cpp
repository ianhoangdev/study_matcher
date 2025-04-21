#include "graphMatcher.h"
#include <algorithm>
#include <unordered_set>

void GraphMatcher::addStudent(const Student& s) {
    std::unordered_set<int> compared;
    for (const auto& course : s.courses) {
        for (int otherUfid : courseMap[course]) {
            if (compared.count(otherUfid)) continue;
            compared.insert(otherUfid);

            const Student& otherStudent = students[otherUfid];
            int courseOverlap = countCourseOverlap(s.courses, otherStudent.courses);
            int prefOverlap = countPreferenceOverlap(s.prefs, otherStudent.prefs);
            int weight = courseOverlap * courseWeight + prefOverlap * prefWeight;

            if (weight > 0) {
                edges[s.ufid][otherUfid] = weight;
                edges[otherUfid][s.ufid] = weight;
            }
        }
        courseMap[course].insert(s.ufid);
    }
    students[s.ufid] = s;
}

std::vector<std::pair<int, int>> GraphMatcher::findTopMatches(int ufid, int topN) {
    std::vector<std::pair<int, int>> result;
    if (!edges.count(ufid)) return result;

    for (const auto& [neighbor, weight] : edges[ufid]) {
        result.emplace_back(neighbor, weight);
    }

    std::sort(result.begin(), result.end(), [](auto& a, auto& b) {
        return b.second < a.second; // descending
    });

    if (result.size() > topN) result.resize(topN);
    return result;
}

int GraphMatcher::countCourseOverlap(const std::set<std::string>& a, const std::set<std::string>& b) const {
    int count = 0;
    for (const auto& course : a) {
        if (b.count(course)) count++;
    }
    return count;
}

int GraphMatcher::countPreferenceOverlap(const Preferences& a, const Preferences& b) const {
    int count = 0;
    if (a.environment == b.environment) count++;
    if (a.group_size == b.group_size) count++;
    if (a.study_style == b.study_style) count++;
    return count;
}
