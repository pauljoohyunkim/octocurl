cmake_minimum_required (VERSION 3.25)

# Name of the Project
project(octocurl)

add_library(conn src/conn.c)
add_library(queue src/queue.c)
add_library(filestring src/filestring.c)

add_executable(octocurl src/octocurl.c)
target_link_libraries(octocurl conn queue filestring -lpthread -lncurses -lm -lcurl)
