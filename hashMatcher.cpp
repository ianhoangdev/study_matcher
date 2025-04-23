#include "hashMatcher.h"
#include <algorithm>

//A function used to add Student object
void HashMatcher::addStudent(const Student& s) {
    students[s.ufid] = s;
    for (const auto& course : s.courses) {
        courseMap[course].insert(s.ufid);
    }
}

//Function used to find Top Matches for students
std::vector<std::pair<int, int>> HashMatcher::findTopMatches(int ufid, int topN) {
    std::vector<std::pair<int, int>> result;
    if (!students.count(ufid)) return result;

    const auto& target = students[ufid];
    std::unordered_set<int> candidates;

    for (const auto& course : target.courses) {
        for (const auto& otherUfid : courseMap[course]) {
            if (otherUfid != ufid) {
                candidates.insert(otherUfid);
            }
        }
    }

    for (const auto& cand : candidates) {
        const auto& other = students[cand];
        int score = countCourseOverlap(target.courses, other.courses) * courseWeight +
                    countPreferenceOverlap(target.prefs, other.prefs) * prefWeight;
        result.push_back({cand, score});
    }

    std::sort(result.begin(), result.end(), [](auto& a, auto& b) {
        return b.second < a.second; // descending
    });

    if (result.size() > topN) result.resize(topN);
    return result;
}

//Count the number of course overlap which will be useful for future calculation
int HashMatcher::countCourseOverlap(const std::set<std::string>& a, const std::set<std::string>& b) const {
    int count = 0;
    for (const auto& course : a) {
        if (b.count(course)) count++;
    }
    return count;
}

//Count the number of study preference overlap which will be useful for future calculation
int HashMatcher::countPreferenceOverlap(const Preferences& a, const Preferences& b) const {
    int count = 0;
    if (a.environment == b.environment) count++;
    if (a.group_size == b.group_size) count++;
    if (a.study_style == b.study_style) count++;
    return count;
}
