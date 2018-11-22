cmake_minimum_required(VERSION 2.8.3)

project(RE216)

find_package (Threads)

#add here source files you need to compile and link to the client app
SET(SRC_CLIENT
	client.c
	P2P.c
	users.c
	socketSetup.c
	)


add_executable(RE216_CLIENT ${SRC_CLIENT})
target_link_libraries (RE216_CLIENT ${CMAKE_THREAD_LIBS_INIT})


#add here source files you need to compile and link to the server app
SET(SRC_SERVER
	channels.c
	users.c
	serverFunctions.c
	P2P.c
	socketSetup.c
    server.c)


add_executable(RE216_SERVER  ${SRC_SERVER})

add_subdirectory(header)
