cmake_minimum_required(VERSION 3.30)
project(study_matcher)

set(CMAKE_CXX_STANDARD 20)

add_executable(study_matcher main.cpp
        hashMatcher.cpp
        graphMatcher.cpp
        types.h
        hashMatcher.h
        graphMatcher.h
        dataGenerator.cpp)
