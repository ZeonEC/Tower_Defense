cmake_minimum_required(VERSION 3.16)
project(TowerDefense LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tous les exécutables iront dans build/ (ou build/bin si tu préfères)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(TheGame)
