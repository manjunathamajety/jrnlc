cmake_minimum_required(VERSION 3.10)
#Project: Journal
#Cmake file
project(journal)
set(CMAKE_CXX_STANDARD 17)


add_executable(journal src/main.cc core/entry.cc)

target_include_directories(journal PRIVATE include )


