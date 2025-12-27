cmake_minimum_required(VERSION 3.10)
#Project: Journal
#Cmake file
project(jrnlc)
set(CMAKE_CXX_STANDARD 17)


add_executable(jrnlc src/main.cc core/timestamp.cc core/backup.cc core/parse.cc core/jrnlmanager.cc core/config.cc core/help.cc core/show.cc)

target_include_directories(jrnlc PRIVATE include )

install(TARGETS jrnlc RUNTIME DESTINATION bin)

install(FILES man-pages/jrnlc.1 DESTINATION share/man/man1)

install(FILES LICENSE DESTINATION share/licenses/jrnlc)
