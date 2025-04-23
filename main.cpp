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
using namespace std;
//g++ main.cpp graphMatcher.cpp hashMatcher.cpp dataGenerator.cpp -o main

void generateDataCSV(const string& filename, int count = 1000);

Student parseStudentLine(const string& line) {
    stringstream ss(line);
    string ufidStr, coursesStr, env, group, style;
    getline(ss, ufidStr, ',');
    getline(ss, coursesStr, ',');
    getline(ss, env, ',');
    getline(ss, group, ',');
    getline(ss, style, ',');

    int ufid = stoi(ufidStr); //ufid is stored as an integer
    set<string> courses;
    stringstream courseSS(coursesStr);
    string course;
    while (getline(courseSS, course, '|')) {
        courses.insert(course);
    }

    Preferences prefs = {env, group, style};
    return {ufid, courses, prefs};
}

vector<Student> loadStudents(const string& filename) {
    vector<Student> students;
    ifstream file(filename);
    string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        students.push_back(parseStudentLine(line));
    }
    return students;
}

double jaccardSimilarity(const vector<pair<int, int>>& a,
                         const vector<pair<int, int>>& b) {
    set<int> setA, setB;
    for (const auto& [id, _] : a) setA.insert(id);
    for (const auto& [id, _] : b) setB.insert(id);

    vector<int> intersection;
    set_intersection(setA.begin(), setA.end(), setB.begin(), setB.end(),
                          back_inserter(intersection));

    vector<int> unionSet;
    set_union(setA.begin(), setA.end(), setB.begin(), setB.end(),
                   back_inserter(unionSet));

    return unionSet.empty() ? 0.0 : static_cast<double>(intersection.size()) / unionSet.size();
}

void generateDataFlow() {
    int count;
    cout << "Enter the number of random students to generate: ";
    cin >> count;

    generateDataCSV("data.csv", count);
    cout << "Generated " << count << " students and saved to data.csv!\n\n";
}

//When we are comparing, we take the first input as the standard
void compareMatchersFlow() {
    const string filename = "data.csv";
    vector<Student> students = loadStudents(filename);

    if (students.empty()) {
        cerr << "Failed to load students from " << filename << "\n";
        return;
    }

    int testUfid = students[0].ufid;

    HashMatcher hashMatcher;
    auto startA = chrono::high_resolution_clock::now();
    for (const auto& s : students) {
        hashMatcher.addStudent(s);
    }
    auto matchesA = hashMatcher.findTopMatches(testUfid);
    auto endA = chrono::high_resolution_clock::now();

    GraphMatcher graphMatcher;
    auto startB = chrono::high_resolution_clock::now();
    for (const auto& s : students) {
        graphMatcher.addStudent(s);
    }
    auto matchesB = graphMatcher.findTopMatches(testUfid);
    auto endB = chrono::high_resolution_clock::now();

    cout << "\nComparing matchers on student " << testUfid << "\n\n";

    cout << "HashMatcher Top Matches:\n";
    for (const auto& [id, score] : matchesA) {
        cout << "  UFID: " << id << ", Score: " << score << "\n";
    }

    cout << "\nGraphMatcher Top Matches:\n";
    for (const auto& [id, score] : matchesB) {
        cout << "  UFID: " << id << ", Score: " << score << "\n";
    }

    auto durationA = chrono::duration_cast<chrono::milliseconds>(endA - startA).count();
    auto durationB = chrono::duration_cast<chrono::milliseconds>(endB - startB).count();

    cout << "\nHashMatcher Time: " << durationA << " ms\n";
    cout << "GraphMatcher Time: " << durationB << " ms\n";

    double similarity = jaccardSimilarity(matchesA, matchesB);
    cout << "\nJaccard Similarity of Top Matches: " << similarity << "\n\n";
}

//Searching single UFID function

void searchSingleUFID(const vector<Student>& students) {
    if (students.empty()) {
        cerr << "No students loaded.\n";
        return;
    }

    unordered_map<int, Student> studentMap;
    for (const auto& s : students) {
        studentMap[s.ufid] = s;
    }

    HashMatcher hashMatcher;
    GraphMatcher graphMatcher;
    for (const auto& s : students) {
        hashMatcher.addStudent(s);
        graphMatcher.addStudent(s);
    }

    cout << "\nChoose matcher:\n";
    cout << "1. HashMatcher\n";
    cout << "2. GraphMatcher\n";
    cout << "Enter choice: ";
    int matcherChoice;
    cin >> matcherChoice;

    cout << "Enter UFID to find matches for: ";
    int ufid;
    cin >> ufid;

    if (!studentMap.count(ufid)) {
        cerr << "UFID not found in the dataset.\n";
        return;
    }

    const Student& student = studentMap[ufid];
    cout << "\nStudent Info:\n";
    cout << "UFID: " << student.ufid << "\n";
    cout << "Courses: ";
    for (auto it = student.courses.begin(); it != student.courses.end(); ++it) {
        cout << *it;
        if (next(it) != student.courses.end()) cout << ", ";
    }
    cout << "\nPreferences:\n";
    cout << "  - Environment: " << student.prefs.environment << "\n";
    cout << "  - Group Size : " << student.prefs.group_size << "\n";
    cout << "  - Study Style: " << student.prefs.study_style << "\n";

    cout << "\nTop Matches:\n";
    vector<pair<int, int>> matches;

    if (matcherChoice == 1) {
        matches = hashMatcher.findTopMatches(ufid);
        cout << "Using HashMatcher:\n";
    } else if (matcherChoice == 2) {
        matches = graphMatcher.findTopMatches(ufid);
        cout << "Using GraphMatcher:\n";
    } else {
        cout << "Invalid matcher choice.\n";
        return;
    }

    for (const auto& [matchUfid, score] : matches) {
        const Student& match = studentMap[matchUfid];

        vector<string> sharedCourses;
        for (const auto& c : student.courses) {
            if (match.courses.count(c)) sharedCourses.push_back(c);
        }

        vector<string> sharedPrefs;
        if (student.prefs.environment == match.prefs.environment)
            sharedPrefs.push_back("environment");
        if (student.prefs.group_size == match.prefs.group_size)
            sharedPrefs.push_back("group_size");
        if (student.prefs.study_style == match.prefs.study_style)
            sharedPrefs.push_back("study_style");

        cout << "\nUFID: " << matchUfid << ", Score: " << score << "\n";
        cout << "   Shared Courses: ";
        if (sharedCourses.empty()) cout << "(none)";
        else for (size_t i = 0; i < sharedCourses.size(); ++i)
            cout << sharedCourses[i] << (i + 1 < sharedCourses.size() ? ", " : "");
        cout << "\n";

        cout << "   Shared Preferences: ";
        if (sharedPrefs.empty()) cout << "(none)";
        else for (size_t i = 0; i < sharedPrefs.size(); ++i)
            cout << sharedPrefs[i] << (i + 1 < sharedPrefs.size() ? ", " : "");
        cout << "\n";
    }
}

//Main part

int main() {
    vector<Student> students;
    bool dataLoaded = false;

    while (true) {
        cout << "Select an option:\n";
        cout << "1. Generate random student data\n";
        cout << "2. Compare HashMatcher and GraphMatcher\n";
        cout << "3. Search Student (HashMatcher or GraphMatcher)\n";
        cout << "4. Exit\n";
        cout << "==============================\n";
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

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
            cout << "Exiting...\n";
            break;
        }
        else {
            cout << "Invalid choice. Please try again.\n\n";
        }
    }

    return 0;
}