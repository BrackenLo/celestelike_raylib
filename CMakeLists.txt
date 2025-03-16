# 12697170160993870887

cmake_minimum_required(VERSION 3.15)
project("celestelike_raylib")

#Project Config Flags:
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#Project Dependencies: 
set(BUILD_EXAMPLES        OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES           OFF CACHE BOOL "" FORCE)
add_subdirectory(external/raylib)

file(GLOB IMGUI_SOURCES "external/imgui/*.cpp" "external/imgui/*.c" "external/imgui/*.hpp" "external/imgui/*.h" "external/imgui/misc/cpp/*.cpp")
add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC external/imgui external/imgui/misc/cpp/)

file(GLOB RLIMGUI_SOURCES "external/rlImGui/*.cpp" "external/rlImGui/*.c" "external/rlImGui/*.hpp" "external/rlImGui/*.h")
add_library(rlImGui ${RLIMGUI_SOURCES})
target_include_directories(rlImGui PUBLIC external/rlImGui)
target_link_libraries(rlImGui PUBLIC  raylib imgui)

add_library(imgui_entt_entity_editor INTERFACE)
target_include_directories(imgui_entt_entity_editor INTERFACE external/imgui_entt_entity_editor)

add_subdirectory(external/magic_enum)

set(JUST_INSTALL_CEREAL   ON                 )
add_subdirectory(external/cereal)

add_subdirectory(external/entt)

#Project Files:
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c" "src/*.hpp" "src/*.h")
add_executable("${PROJECT_NAME}" ${SOURCES})
target_link_libraries("${PROJECT_NAME}" PRIVATE  raylib imgui rlImGui imgui_entt_entity_editor magic_enum cereal EnTT)
