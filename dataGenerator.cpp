//
// Created by Quang Dat Hoang on 4/21/25.
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

void generateDataCSV(const std::string& filename, int count = 50) {
    std::vector<std::string> coursePool = {
        "COP3502", "MAC2311", "PHY2048", "CIS4930", "STA2023",
        "COP3530", "CDA3101", "CEN3031", "EEL3701", "COP4600"
    };
    std::vector<std::string> environments = {"quiet", "moderate", "lively"};
    std::vector<std::string> groupSizes = {"small", "medium", "large"};
    std::vector<std::string> studyStyles = {"focused", "casual", "interactive"};

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "❌ Could not create file: " << filename << "\n";
        return;
    }

    file << "ufid,courses,environment,group_size,study_style\n";
    std::mt19937 rng(std::random_device{}());

    for (int i = 0; i < count; ++i) {
        int ufid = 10000000 + i;

        // Random courses
        std::shuffle(coursePool.begin(), coursePool.end(), rng);
        int numCourses = 3 + rng() % 3;
        std::string courseList = coursePool[0];
        for (int j = 1; j < numCourses; ++j) {
            courseList += "|" + coursePool[j];
        }

        // Random preferences
        std::string environment = environments[rng() % environments.size()];
        std::string group = groupSizes[rng() % groupSizes.size()];
        std::string style = studyStyles[rng() % studyStyles.size()];

        file << ufid << "," << courseList << "," << environment << "," << group << "," << style << "\n";
    }

    file.close();
    std::cout << "✅ Generated " << count << " entries in " << filename << "\n";
}
