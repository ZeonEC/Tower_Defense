find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(TowerDefense ${SRC})

# DIT AU COMPILO où trouver les .hpp
target_include_directories(TowerDefense
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(TowerDefense
    PRIVATE
    sfml-graphics sfml-window sfml-system sfml-audio
)
