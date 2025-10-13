cmake_minimum_required(VERSION 4.0)

include(FetchContent)


project(CHIP-8-Emulator C)
set(CMAKE_C_STANDARD 23) # Requires C23 standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

find_package(glfw3 3.4 QUIET)
if (NOT glfw3_FOUND)
  FetchContent_Declare(
    glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw
  )
  FetchContent_GetProperties(glfw3)
  if (NOT glfw3_POPULATED)
      set(FETCHCONTENT_QUIET NO)
      FetchContent_MakeAvailable(glfw3)
  endif()
endif()

# GLAD
FetchContent_Declare(
    glad
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/Dav1dde/glad/archive/refs/tags/v2.0.8.zip
)
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(glad)

    add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)
    glad_add_library(
      glad REPRODUCIBLE EXCLUDE_FROM_ALL LOADER API gl:compatibility=3.3
    )
endif()
set_target_properties(glad PROPERTIES FOLDER "Dependencies")



# Source files
list(APPEND   SRC_FILES 
  src/main.c 
  src/engine/chip8.c
  src/engine/opcodes.c
  src/gfx/window.c
  src/gfx/renderer.c
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} glad)

list(
  APPEND EXTRA_INCLUDES 

  "${PROJECT_SOURCE_DIR}/src"
  "${glfw3_SOURCE_DIR}/include"
  "${glad_SOURCE_DIR}/glad/include"
)

target_include_directories(${PROJECT_NAME} PRIVATE ${EXTRA_INCLUDES})


