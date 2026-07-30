cmake_minimum_required(VERSION 3.24)
project(RaylibDungeon CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 6.0
        GIT_SHALLOW TRUE
)
set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

# Prefer tarball over git repo clone
FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

FetchContent_MakeAvailable(raylib nlohmann_json)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*)
add_executable(main ${SOURCES}
        src/modules/menu/MenuModule.cpp
        src/constants/ScreenConstants.h
        src/constants/ScreenConstants.h)
target_link_libraries(main PRIVATE raylib nlohmann_json::nlohmann_json)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:main>/resources
        COMMENT "Copying resources to output directory"
)
