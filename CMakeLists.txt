cmake_minimum_required(VERSION 3.20)
project(algds LANGUAGES C)


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()


option(ALGDS_SANITIZE "Enable Address and Undefined Behavior sanitizers" OFF)

if(ALGDS_SANITIZE)
    # 2. Create the interface target
    add_library(project_sanitizers INTERFACE)

    # 3. Add compiler and linker flags
    target_compile_options(project_sanitizers INTERFACE -fsanitize=address,undefined -g -O1)
    target_link_options(project_sanitizers INTERFACE -fsanitize=address,undefined)
    
    message(STATUS "Sanitizers are ENABLED")
endif()

add_library(project_warnings INTERFACE)

target_compile_options(project_warnings INTERFACE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)

add_library(algds STATIC
  src/version.c
)
target_include_directories(algds PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(algds PRIVATE project_warnings)

add_executable(smoke_version examples/smoke/main.c)
target_link_libraries(smoke_version PRIVATE algds project_warnings)

if(ALGDS_SANITIZE)
    target_link_libraries(algds PRIVATE project_sanitizers)
endif()


