#------------------------------------------------#
# Fichier CMakeLists.txt pour le projet TowerDefense #
#------------------------------------------------#

#--- Configuration de CMake ---#
cmake_minimum_required(VERSION 3.16) 

project(TowerDefense LANGUAGES CXX) 
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

# Tous les exécutables iront dans build/ (ou build/bin si tu préfères) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}) 
file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp") 

#--- Configuration des libraires annexes ---#

# Trouve SFML (paquet libsfml-dev)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

#-------------------------------------------#


# Crée l'exécutable à partir des fichiers sources
add_executable(main
  src/main.cpp
  src/render.cpp
  src/projectile.cpp
  src/enemy.cpp
  src/tower.cpp
)

#--- Lier les librairies au bon target ---# 

target_link_libraries(main PRIVATE sfml-graphics sfml-window sfml-system)

#-------------------------------------------#

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)