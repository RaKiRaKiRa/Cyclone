# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/raki/Desktop/C++/MyProject/Cyclone/src/base

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test

# Include any dependencies generated for this target.
include test/CMakeFiles/log_test.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/log_test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/log_test.dir/flags.make

test/CMakeFiles/log_test.dir/log_test.o: test/CMakeFiles/log_test.dir/flags.make
test/CMakeFiles/log_test.dir/log_test.o: log_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/raki/Desktop/C++/MyProject/Cyclone/src/base/test/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/log_test.dir/log_test.o"
	cd /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/log_test.dir/log_test.o -c /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test/log_test.cc

test/CMakeFiles/log_test.dir/log_test.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/log_test.dir/log_test.i"
	cd /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test/log_test.cc > CMakeFiles/log_test.dir/log_test.i

test/CMakeFiles/log_test.dir/log_test.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/log_test.dir/log_test.s"
	cd /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test/log_test.cc -o CMakeFiles/log_test.dir/log_test.s

# Object files for target log_test
log_test_OBJECTS = \
"CMakeFiles/log_test.dir/log_test.o"

# External object files for target log_test
log_test_EXTERNAL_OBJECTS =

test/log_test: test/CMakeFiles/log_test.dir/log_test.o
test/log_test: test/CMakeFiles/log_test.dir/build.make
test/log_test: libCyclone_base.a
test/log_test: test/CMakeFiles/log_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/raki/Desktop/C++/MyProject/Cyclone/src/base/test/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable log_test"
	cd /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/log_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/log_test.dir/build: test/log_test

.PHONY : test/CMakeFiles/log_test.dir/build

test/CMakeFiles/log_test.dir/clean:
	cd /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test/test && $(CMAKE_COMMAND) -P CMakeFiles/log_test.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/log_test.dir/clean

test/CMakeFiles/log_test.dir/depend:
	cd /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/raki/Desktop/C++/MyProject/Cyclone/src/base /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test/test /home/raki/Desktop/C++/MyProject/Cyclone/src/base/test/test/CMakeFiles/log_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/log_test.dir/depend

