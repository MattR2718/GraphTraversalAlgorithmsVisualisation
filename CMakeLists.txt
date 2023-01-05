cmake_minimum_required(VERSION 3.0.0)
project(GraphTraversal VERSION 0.1.0)

find_package(SFML REQUIRED COMPONENTS graphics system window)
find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
    imgui::imgui
    sfml-graphics sfml-system sfml-window
    ImGui-SFML::ImGui-SFML
    implot::implot
)