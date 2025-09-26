cmake_minimum_required( VERSION 3.5 )
project(Tower_Defence)

set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

find_package(Boost REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/Hpp)
include_directories(${Boost_INCLUDE_DIRS})




set (HEADERS
    Hpp/Astar.hpp
    Hpp/main.hpp
    Hpp/tower.hpp
    Hpp/ennemy.hpp
    Hpp/perso.hpp
)

set (SOURCES
    Cpp/Astar.cpp
    Cpp/main.cpp
    Cpp/tower.cpp
    Cpp/ennemy.cpp
    Cpp/perso.cpp

)



add_executable(main ${SOURCES})



target_link_libraries(main sfml-graphics sfml-window sfml-system)