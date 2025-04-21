#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <set>
#include <algorithm>
#include "types.h"
#include "hashMatcher.h"
#include "graphMatcher.h"

void generateDataCSV(const std::string& filename, int count = 1000);

Student parseStudentLine(const std::string& line) {
    std::stringstream ss(line);
    std::string ufidStr, coursesStr, env, group, style;
    std::getline(ss, ufidStr, ',');
    std::getline(ss, coursesStr, ',');
    std::getline(ss, env, ',');
    std::getline(ss, group, ',');
    std::getline(ss, style, ',');

    int ufid = std::stoi(ufidStr);
    std::set<std::string> courses;
    std::stringstream courseSS(coursesStr);
    std::string course;
    while (std::getline(courseSS, course, '|')) {
        courses.insert(course);
    }

    Preferences prefs = {env, group, style};
    return {ufid, courses, prefs};
}

std::vector<Student> loadStudents(const std::string& filename) {
    std::vector<Student> students;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        students.push_back(parseStudentLine(line));
    }
    return students;
}

double jaccardSimilarity(const std::vector<std::pair<int, int>>& a,
                         const std::vector<std::pair<int, int>>& b) {
    std::set<int> setA, setB;
    for (const auto& [id, _] : a) setA.insert(id);
    for (const auto& [id, _] : b) setB.insert(id);

    std::vector<int> intersection;
    std::set_intersection(setA.begin(), setA.end(), setB.begin(), setB.end(),
                          std::back_inserter(intersection));

    std::vector<int> unionSet;
    std::set_union(setA.begin(), setA.end(), setB.begin(), setB.end(),
                   std::back_inserter(unionSet));

    return unionSet.empty() ? 0.0 : static_cast<double>(intersection.size()) / unionSet.size();
}

void generateDataFlow() {
    int count;
    std::cout << "Enter the number of random students to generate: ";
    std::cin >> count;

    generateDataCSV("data.csv", count);
    std::cout << "Generated " << count << " students and saved to data.csv!\n\n";
}

void compareMatchersFlow() {
    const std::string filename = "data.csv";
    std::vector<Student> students = loadStudents(filename);

    if (students.empty()) {
        std::cerr << "Failed to load students from " << filename << "\n";
        return;
    }

    int testUfid = students[0].ufid;

    HashMatcher hashMatcher;
    auto startA = std::chrono::high_resolution_clock::now();
    for (const auto& s : students) {
        hashMatcher.addStudent(s);
    }
    auto matchesA = hashMatcher.findTopMatches(testUfid);
    auto endA = std::chrono::high_resolution_clock::now();

    GraphMatcher graphMatcher;
    auto startB = std::chrono::high_resolution_clock::now();
    for (const auto& s : students) {
        graphMatcher.addStudent(s);
    }
    auto matchesB = graphMatcher.findTopMatches(testUfid);
    auto endB = std::chrono::high_resolution_clock::now();

    std::cout << "\nComparing matchers on student " << testUfid << "\n\n";

    std::cout << "HashMatcher Top Matches:\n";
    for (const auto& [id, score] : matchesA) {
        std::cout << "  UFID: " << id << ", Score: " << score << "\n";
    }

    std::cout << "\nGraphMatcher Top Matches:\n";
    for (const auto& [id, score] : matchesB) {
        std::cout << "  UFID: " << id << ", Score: " << score << "\n";
    }

    auto durationA = std::chrono::duration_cast<std::chrono::milliseconds>(endA - startA).count();
    auto durationB = std::chrono::duration_cast<std::chrono::milliseconds>(endB - startB).count();

    std::cout << "\nHashMatcher Time: " << durationA << " ms\n";
    std::cout << "GraphMatcher Time: " << durationB << " ms\n";

    double similarity = jaccardSimilarity(matchesA, matchesB);
    std::cout << "\nJaccard Similarity of Top Matches: " << similarity << "\n\n";
}



void searchSingleUFID(const std::vector<Student>& students) {
    if (students.empty()) {
        std::cerr << "No students loaded.\n";
        return;
    }

    std::unordered_map<int, Student> studentMap;
    for (const auto& s : students) {
        studentMap[s.ufid] = s;
    }

    HashMatcher hashMatcher;
    GraphMatcher graphMatcher;
    for (const auto& s : students) {
        hashMatcher.addStudent(s);
        graphMatcher.addStudent(s);
    }

    std::cout << "\nChoose matcher:\n";
    std::cout << "1. HashMatcher\n";
    std::cout << "2. GraphMatcher\n";
    std::cout << "Enter choice: ";
    int matcherChoice;
    std::cin >> matcherChoice;

    std::cout << "Enter UFID to find matches for: ";
    int ufid;
    std::cin >> ufid;

    if (!studentMap.count(ufid)) {
        std::cerr << "UFID not found in the dataset.\n";
        return;
    }

    const Student& student = studentMap[ufid];
    std::cout << "\nStudent Info:\n";
    std::cout << "UFID: " << student.ufid << "\n";
    std::cout << "Courses: ";
    for (auto it = student.courses.begin(); it != student.courses.end(); ++it) {
        std::cout << *it;
        if (std::next(it) != student.courses.end()) std::cout << ", ";
    }
    std::cout << "\nPreferences:\n";
    std::cout << "  - Environment: " << student.prefs.environment << "\n";
    std::cout << "  - Group Size : " << student.prefs.group_size << "\n";
    std::cout << "  - Study Style: " << student.prefs.study_style << "\n";

    std::cout << "\nTop Matches:\n";
    std::vector<std::pair<int, int>> matches;

    if (matcherChoice == 1) {
        matches = hashMatcher.findTopMatches(ufid);
        std::cout << "Using HashMatcher:\n";
    } else if (matcherChoice == 2) {
        matches = graphMatcher.findTopMatches(ufid);
        std::cout << "Using GraphMatcher:\n";
    } else {
        std::cout << "Invalid matcher choice.\n";
        return;
    }

    for (const auto& [matchUfid, score] : matches) {
        const Student& match = studentMap[matchUfid];

        std::vector<std::string> sharedCourses;
        for (const auto& c : student.courses) {
            if (match.courses.count(c)) sharedCourses.push_back(c);
        }

        std::vector<std::string> sharedPrefs;
        if (student.prefs.environment == match.prefs.environment)
            sharedPrefs.push_back("environment");
        if (student.prefs.group_size == match.prefs.group_size)
            sharedPrefs.push_back("group_size");
        if (student.prefs.study_style == match.prefs.study_style)
            sharedPrefs.push_back("study_style");

        std::cout << "\nUFID: " << matchUfid << ", Score: " << score << "\n";
        std::cout << "   Shared Courses: ";
        if (sharedCourses.empty()) std::cout << "(none)";
        else for (size_t i = 0; i < sharedCourses.size(); ++i)
            std::cout << sharedCourses[i] << (i + 1 < sharedCourses.size() ? ", " : "");
        std::cout << "\n";

        std::cout << "   Shared Preferences: ";
        if (sharedPrefs.empty()) std::cout << "(none)";
        else for (size_t i = 0; i < sharedPrefs.size(); ++i)
            std::cout << sharedPrefs[i] << (i + 1 < sharedPrefs.size() ? ", " : "");
        std::cout << "\n";
    }
}



int main() {
    std::vector<Student> students;
    bool dataLoaded = false;

    while (true) {
        std::cout << "Select an option:\n";
        std::cout << "1. Generate random student data\n";
        std::cout << "2. Compare HashMatcher and GraphMatcher\n";
        std::cout << "3. Search Student (HashMatcher or GraphMatcher)\n";
        std::cout << "4. Exit\n";
        std::cout << "==============================\n";
        std::cout << "Enter choice: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            generateDataFlow();
            // After generating, reload data
            students = loadStudents("data.csv");
            dataLoaded = true;
        }
        else if (choice == 2) {
            if (!dataLoaded) {
                students = loadStudents("data.csv");
                dataLoaded = true;
            }
            compareMatchersFlow();
        }
        else if (choice == 3) {
            if (!dataLoaded) {
                students = loadStudents("data.csv");
                dataLoaded = true;
            }
            searchSingleUFID(students);
        }
        else if (choice == 4) {
            std::cout << "Exiting...\n";
            break;
        }
        else {
            std::cout << "Invalid choice. Please try again.\n\n";
        }
    }

    return 0;
}