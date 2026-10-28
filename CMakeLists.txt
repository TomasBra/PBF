cmake_minimum_required(VERSION 4.3)
project(PositionBasedFluids, LANGUAGES C CXX)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.h")

file(GLOB_RECURSE SOURCES_GLAD
        "${PROJECT_SOURCE_DIR}/libs/glad/*.c"
        "${PROJECT_SOURCE_DIR}/libs/glad/*.cpp"
        "${PROJECT_SOURCE_DIR}/libs/glad/*.h")

file(GLOB_RECURSE SOURCES_IMGUI
        "${PROJECT_SOURCE_DIR}/libs/imgui/*.c"
        "${PROJECT_SOURCE_DIR}/libs/imgui/*.cpp"
        "${PROJECT_SOURCE_DIR}/libs/imgui/*.h")

file(GLOB_RECURSE SOURCES_GLM
        "${PROJECT_SOURCE_DIR}/libs/glm/*.c"
        "${PROJECT_SOURCE_DIR}/libs/glm/*.cpp"
        "${PROJECT_SOURCE_DIR}/libs/glm/*.h")

file(GLOB_RECURSE SOURCES_STB
        "${PROJECT_SOURCE_DIR}/libs/stb_image/*.h")

include_directories(PositionBasedFluids src libs)

add_library(glad SHARED ${SOURCES_GLAD})

add_library(imgui SHARED ${SOURCES_IMGUI})

add_library(glm SHARED ${SOURCES_GLM})

add_executable(PositionBasedFluids ${SOURCES})

target_link_libraries(PositionBasedFluids
        glfw
        OpenGL::GL
        imgui
        glad
        glm)