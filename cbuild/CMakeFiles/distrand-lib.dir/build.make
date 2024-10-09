# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/memory/C/distrand

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/memory/C/distrand/cbuild

# Include any dependencies generated for this target.
include CMakeFiles/distrand-lib.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/distrand-lib.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/distrand-lib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/distrand-lib.dir/flags.make

# Object files for target distrand-lib
distrand__lib_OBJECTS =

# External object files for target distrand-lib
distrand__lib_EXTERNAL_OBJECTS = \
"/mnt/memory/C/distrand/cbuild/CMakeFiles/distrand-lib-objects.dir/src/distributed/edsrm/2rng.c.o" \
"/mnt/memory/C/distrand/cbuild/CMakeFiles/distrand-lib-objects.dir/src/distributed/edsrm/mnt.c.o" \
"/mnt/memory/C/distrand/cbuild/CMakeFiles/distrand-lib-objects.dir/src/distributed/ziggurat/mnt.c.o" \
"/mnt/memory/C/distrand/cbuild/CMakeFiles/distrand-lib-objects.dir/src/prob_eq/dbd.c.o" \
"/mnt/memory/C/distrand/cbuild/CMakeFiles/distrand-lib-objects.dir/src/uniform/mt19937_64.c.o" \
"/mnt/memory/C/distrand/cbuild/CMakeFiles/distrand-lib-objects.dir/src/uniform/multiplicative.c.o"

libdistrand-lib.a: CMakeFiles/distrand-lib-objects.dir/src/distributed/edsrm/2rng.c.o
libdistrand-lib.a: CMakeFiles/distrand-lib-objects.dir/src/distributed/edsrm/mnt.c.o
libdistrand-lib.a: CMakeFiles/distrand-lib-objects.dir/src/distributed/ziggurat/mnt.c.o
libdistrand-lib.a: CMakeFiles/distrand-lib-objects.dir/src/prob_eq/dbd.c.o
libdistrand-lib.a: CMakeFiles/distrand-lib-objects.dir/src/uniform/mt19937_64.c.o
libdistrand-lib.a: CMakeFiles/distrand-lib-objects.dir/src/uniform/multiplicative.c.o
libdistrand-lib.a: CMakeFiles/distrand-lib.dir/build.make
libdistrand-lib.a: CMakeFiles/distrand-lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/memory/C/distrand/cbuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking C static library libdistrand-lib.a"
	$(CMAKE_COMMAND) -P CMakeFiles/distrand-lib.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/distrand-lib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/distrand-lib.dir/build: libdistrand-lib.a
.PHONY : CMakeFiles/distrand-lib.dir/build

CMakeFiles/distrand-lib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/distrand-lib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/distrand-lib.dir/clean

CMakeFiles/distrand-lib.dir/depend:
	cd /mnt/memory/C/distrand/cbuild && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/memory/C/distrand /mnt/memory/C/distrand /mnt/memory/C/distrand/cbuild /mnt/memory/C/distrand/cbuild /mnt/memory/C/distrand/cbuild/CMakeFiles/distrand-lib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/distrand-lib.dir/depend
