cmake_minimum_required(VERSION 3.15)
project("celestelike_raylib" CXX)

set(CMAKE_BUILD_TYPE Debug)

include(FetchContent)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#======================================================================

# Fetch Raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
FetchContent_Declare(raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        5.5
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(raylib)

#----------------------------------------------------------
# Fetch EnTT
FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG        v3.14.0
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(EnTT)

#----------------------------------------------------------
# Fetch magic_enum
FetchContent_Declare(magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
  GIT_TAG        v0.9.7
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(magic_enum)

#----------------------------------------------------------
# Fetch cereal
FetchContent_Declare(cereal
  GIT_REPOSITORY https://github.com/USCiLab/cereal.git
  GIT_TAG        v1.3.2
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
)
set(JUST_INSTALL_CEREAL ON)
FetchContent_MakeAvailable(cereal)

#----------------------------------------------------------

set(IMGUI_PATH "${CMAKE_CURRENT_SOURCE_DIR}/fetch_external/imgui")

# Fetch and populate ImGui
FetchContent_Populate(imgui
  URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.6.zip
  SOURCE_DIR ${IMGUI_PATH}
)

set(IMGUI_HEADER_FILES
  "${IMGUI_PATH}/imgui.h"
  "${IMGUI_PATH}/imconfig.h"
  "${IMGUI_PATH}/imgui_internal.h"
  "${IMGUI_PATH}/imstb_rectpack.h"
  "${IMGUI_PATH}/imstb_textedit.h"
  "${IMGUI_PATH}/imstb_truetype.h"
)

set(IMGUI_SRC_FILES
  "${IMGUI_PATH}/imgui.cpp"
  "${IMGUI_PATH}/imgui_draw.cpp"
  "${IMGUI_PATH}/imgui_widgets.cpp"
  "${IMGUI_PATH}/imgui_demp.cpp"
)

add_library(imgui PUBLIC)
target_sources(imgui ${IMGUI_HEADER_FILES})
target_sources(imgui ${IMGUI_SRC_FILES})

target_include_directories(imgui PUBLIC
  ${IMGUI_PATH}
)


#----------------------------------------------------------
# Fetch and populate RlImGui
FetchContent_Populate(rl_imgui
  URL https://github.com/raylib-extras/rlImGui/archive/refs/heads/main.zip
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/fetch_external/rl_imgui
)

file(GLOB RL_IMGUI_SRC_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/fetch_external/rl_imgui/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/fetch_external/rl_imgui/*.cpp"
)

add_library(rl_imgui ${RL_IMGUI_SRC_FILES})
target_include_directories(rl_imgui INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/fetch_external/rl_imgui")
target_link_libraries(rl_imgui 
  PRIVATE
  raylib
  imgui
)

#----------------------------------------------------------
# Include imgui_entt_entity_editor

#======================================================================

include_directories("${CMAKE_SOURCE_DIR}/include/")
file(GLOB SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable("${PROJECT_NAME}" ${SRC_FILES})

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  raylib
  EnTT
  magic_enum
  cereal
  imgui
  rl_imgui
)


target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_SOURCE_DIR}/external/imgui_entt_entity_editor")

