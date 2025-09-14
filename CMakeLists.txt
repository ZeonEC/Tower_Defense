cmake_minimum_required(VERSION 3.16) 

project(TowerDefense LANGUAGES CXX) 
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

# Tous les exécutables iront dans build/ (ou build/bin si tu préfères) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}) 
file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp") 


add_executable(main
  src/main.cpp
  src/render.cpp
  src/projectile.cpp
  src/ennemie1.cpp
  src/tower.cpp
)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)