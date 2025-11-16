cmake_minimum_required(VERSION 3.10)
#Project: Journal
#Cmake file
project(jrnl)
set(CMAKE_CXX_STANDARD 17)


add_executable(jrnl src/main.cc core/timestamp.cc core/parse.cc core/jrnlmanager.cc core/config.cc core/show.cc)

target_include_directories(jrnl PRIVATE include )

install(TARGETS jrnl RUNTIME DESTINATION bin)
