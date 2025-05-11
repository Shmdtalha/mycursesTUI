cmake_minimum_required(VERSION 3.10)
project(MycursesTUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Curses REQUIRED)

# Add include directories
include_directories(${CURSES_INCLUDE_DIR})

# Add source files
set(SOURCES
    form_test.cpp
    form_classes.h
)

# Create executable
add_executable(form_test ${SOURCES})

# Link libraries
target_link_libraries(form_test ${CURSES_LIBRARIES} form)

# Add compiler flags
if(MSVC)
    target_compile_options(form_test PRIVATE /W4)
else()
    target_compile_options(form_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Add include directories for ncurses
target_include_directories(form_test PRIVATE 
    ${CURSES_INCLUDE_DIR}
    /usr/include/ncurses
    /usr/local/include/ncurses
) 