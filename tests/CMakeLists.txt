cmake_minimum_required(VERSION 3.14)
project(PipexTests)

include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/heads/main.zip)
FetchContent_MakeAvailable(googletest)

# 1. Source files (EXCLUDE main.c)
list(APPEND SRC_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/../find_path.c" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../process.c"
)

# 2. Setup the Test Executable
add_executable(pipex_torture pipex_test.cpp ${SRC_FILES})

# 3. Include Directories
target_include_directories(pipex_torture PRIVATE 
    ../ 
    ../libft/
)

# 4. THE MAGIC: Intercept System Calls
# We wrap malloc (memory), exit (termination), and execve/fork (process control)
target_link_options(pipex_torture PRIVATE 
    "-Wl,--wrap=malloc" 
    "-Wl,--wrap=exit" 
    "-Wl,--wrap=execve" 
    "-Wl,--wrap=fork"
)

# 5. Link Libraries
target_link_libraries(pipex_torture 
    gtest_main 
    "${CMAKE_CURRENT_SOURCE_DIR}/../libft/libft.a" 
    pthread
)