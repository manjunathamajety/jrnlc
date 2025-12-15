cmake_minimum_required(VERSION 3.10)
#Project: Journal
#Cmake file
project(jrnl)
set(CMAKE_CXX_STANDARD 17)


add_executable(jrnl src/main.cc core/timestamp.cc core/backup.cc core/parse.cc core/jrnlmanager.cc core/config.cc core/help.cc core/show.cc)

target_include_directories(jrnl PRIVATE include )

install(TARGETS jrnl RUNTIME DESTINATION bin)

install(FILES man-pages/jrnl.1 DESTINATION share/man/man1)

install(FILES LICENSE DESTINATION share/licenses/jrnl)
