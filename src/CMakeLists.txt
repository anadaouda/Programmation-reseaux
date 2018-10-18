cmake_minimum_required(VERSION 2.8.3)

project(RE216)

#add here source files you need to compile and link to the client app
SET(SRC_CLIENT
	client.c
	)


add_executable(RE216_CLIENT ${SRC_CLIENT})


#add here source files you need to compile and link to the server app
SET(SRC_SERVER
	users.c
        server.c)


add_executable(RE216_SERVER  ${SRC_SERVER})

add_subdirectory(header)


