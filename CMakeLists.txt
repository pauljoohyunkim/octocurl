cmake_minimum_required (VERSION 3.25)

# Name of the Project
project(octocurl)

add_executable(octocurl src/octocurl.c src/conn.c src/queue.c src/filestring.c)
target_link_libraries(octocurl -lpthread -lncurses -lm -lcurl)
