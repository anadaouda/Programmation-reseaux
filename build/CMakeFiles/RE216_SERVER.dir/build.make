# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/ana/Documents/S7/Programmation réseaux/src"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/ana/Documents/S7/Programmation réseaux/build"

# Include any dependencies generated for this target.
include CMakeFiles/RE216_SERVER.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/RE216_SERVER.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/RE216_SERVER.dir/flags.make

CMakeFiles/RE216_SERVER.dir/channels.c.o: CMakeFiles/RE216_SERVER.dir/flags.make
CMakeFiles/RE216_SERVER.dir/channels.c.o: /home/ana/Documents/S7/Programmation\ réseaux/src/channels.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/ana/Documents/S7/Programmation réseaux/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/RE216_SERVER.dir/channels.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/RE216_SERVER.dir/channels.c.o   -c "/home/ana/Documents/S7/Programmation réseaux/src/channels.c"

CMakeFiles/RE216_SERVER.dir/channels.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/RE216_SERVER.dir/channels.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/ana/Documents/S7/Programmation réseaux/src/channels.c" > CMakeFiles/RE216_SERVER.dir/channels.c.i

CMakeFiles/RE216_SERVER.dir/channels.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/RE216_SERVER.dir/channels.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/ana/Documents/S7/Programmation réseaux/src/channels.c" -o CMakeFiles/RE216_SERVER.dir/channels.c.s

CMakeFiles/RE216_SERVER.dir/channels.c.o.requires:

.PHONY : CMakeFiles/RE216_SERVER.dir/channels.c.o.requires

CMakeFiles/RE216_SERVER.dir/channels.c.o.provides: CMakeFiles/RE216_SERVER.dir/channels.c.o.requires
	$(MAKE) -f CMakeFiles/RE216_SERVER.dir/build.make CMakeFiles/RE216_SERVER.dir/channels.c.o.provides.build
.PHONY : CMakeFiles/RE216_SERVER.dir/channels.c.o.provides

CMakeFiles/RE216_SERVER.dir/channels.c.o.provides.build: CMakeFiles/RE216_SERVER.dir/channels.c.o


CMakeFiles/RE216_SERVER.dir/users.c.o: CMakeFiles/RE216_SERVER.dir/flags.make
CMakeFiles/RE216_SERVER.dir/users.c.o: /home/ana/Documents/S7/Programmation\ réseaux/src/users.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/ana/Documents/S7/Programmation réseaux/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/RE216_SERVER.dir/users.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/RE216_SERVER.dir/users.c.o   -c "/home/ana/Documents/S7/Programmation réseaux/src/users.c"

CMakeFiles/RE216_SERVER.dir/users.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/RE216_SERVER.dir/users.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/ana/Documents/S7/Programmation réseaux/src/users.c" > CMakeFiles/RE216_SERVER.dir/users.c.i

CMakeFiles/RE216_SERVER.dir/users.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/RE216_SERVER.dir/users.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/ana/Documents/S7/Programmation réseaux/src/users.c" -o CMakeFiles/RE216_SERVER.dir/users.c.s

CMakeFiles/RE216_SERVER.dir/users.c.o.requires:

.PHONY : CMakeFiles/RE216_SERVER.dir/users.c.o.requires

CMakeFiles/RE216_SERVER.dir/users.c.o.provides: CMakeFiles/RE216_SERVER.dir/users.c.o.requires
	$(MAKE) -f CMakeFiles/RE216_SERVER.dir/build.make CMakeFiles/RE216_SERVER.dir/users.c.o.provides.build
.PHONY : CMakeFiles/RE216_SERVER.dir/users.c.o.provides

CMakeFiles/RE216_SERVER.dir/users.c.o.provides.build: CMakeFiles/RE216_SERVER.dir/users.c.o


CMakeFiles/RE216_SERVER.dir/server.c.o: CMakeFiles/RE216_SERVER.dir/flags.make
CMakeFiles/RE216_SERVER.dir/server.c.o: /home/ana/Documents/S7/Programmation\ réseaux/src/server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/ana/Documents/S7/Programmation réseaux/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/RE216_SERVER.dir/server.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/RE216_SERVER.dir/server.c.o   -c "/home/ana/Documents/S7/Programmation réseaux/src/server.c"

CMakeFiles/RE216_SERVER.dir/server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/RE216_SERVER.dir/server.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/ana/Documents/S7/Programmation réseaux/src/server.c" > CMakeFiles/RE216_SERVER.dir/server.c.i

CMakeFiles/RE216_SERVER.dir/server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/RE216_SERVER.dir/server.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/ana/Documents/S7/Programmation réseaux/src/server.c" -o CMakeFiles/RE216_SERVER.dir/server.c.s

CMakeFiles/RE216_SERVER.dir/server.c.o.requires:

.PHONY : CMakeFiles/RE216_SERVER.dir/server.c.o.requires

CMakeFiles/RE216_SERVER.dir/server.c.o.provides: CMakeFiles/RE216_SERVER.dir/server.c.o.requires
	$(MAKE) -f CMakeFiles/RE216_SERVER.dir/build.make CMakeFiles/RE216_SERVER.dir/server.c.o.provides.build
.PHONY : CMakeFiles/RE216_SERVER.dir/server.c.o.provides

CMakeFiles/RE216_SERVER.dir/server.c.o.provides.build: CMakeFiles/RE216_SERVER.dir/server.c.o


# Object files for target RE216_SERVER
RE216_SERVER_OBJECTS = \
"CMakeFiles/RE216_SERVER.dir/channels.c.o" \
"CMakeFiles/RE216_SERVER.dir/users.c.o" \
"CMakeFiles/RE216_SERVER.dir/server.c.o"

# External object files for target RE216_SERVER
RE216_SERVER_EXTERNAL_OBJECTS =

RE216_SERVER: CMakeFiles/RE216_SERVER.dir/channels.c.o
RE216_SERVER: CMakeFiles/RE216_SERVER.dir/users.c.o
RE216_SERVER: CMakeFiles/RE216_SERVER.dir/server.c.o
RE216_SERVER: CMakeFiles/RE216_SERVER.dir/build.make
RE216_SERVER: CMakeFiles/RE216_SERVER.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/ana/Documents/S7/Programmation réseaux/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable RE216_SERVER"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/RE216_SERVER.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/RE216_SERVER.dir/build: RE216_SERVER

.PHONY : CMakeFiles/RE216_SERVER.dir/build

CMakeFiles/RE216_SERVER.dir/requires: CMakeFiles/RE216_SERVER.dir/channels.c.o.requires
CMakeFiles/RE216_SERVER.dir/requires: CMakeFiles/RE216_SERVER.dir/users.c.o.requires
CMakeFiles/RE216_SERVER.dir/requires: CMakeFiles/RE216_SERVER.dir/server.c.o.requires

.PHONY : CMakeFiles/RE216_SERVER.dir/requires

CMakeFiles/RE216_SERVER.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/RE216_SERVER.dir/cmake_clean.cmake
.PHONY : CMakeFiles/RE216_SERVER.dir/clean

CMakeFiles/RE216_SERVER.dir/depend:
	cd "/home/ana/Documents/S7/Programmation réseaux/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/ana/Documents/S7/Programmation réseaux/src" "/home/ana/Documents/S7/Programmation réseaux/src" "/home/ana/Documents/S7/Programmation réseaux/build" "/home/ana/Documents/S7/Programmation réseaux/build" "/home/ana/Documents/S7/Programmation réseaux/build/CMakeFiles/RE216_SERVER.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/RE216_SERVER.dir/depend

