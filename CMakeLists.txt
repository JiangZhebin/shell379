cmake_minimum_required(VERSION 2.8.12)
project(shell379)
set(CMAKE_CXX_STANDARD 11)
add_library(shell_data_manager STATIC
process.cpp
process_manager.cpp)
add_executable(${PROJECT_NAME} shell379_main.cpp)
target_link_libraries(${PROJECT_NAME} shell_data_manager -lpthread)