# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

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
CMAKE_SOURCE_DIR = /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2

# Include any dependencies generated for this target.
include tests/CMakeFiles/gctest.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/gctest.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/gctest.dir/flags.make

tests/CMakeFiles/gctest.dir/test.c.o: tests/CMakeFiles/gctest.dir/flags.make
tests/CMakeFiles/gctest.dir/test.c.o: tests/test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tests/CMakeFiles/gctest.dir/test.c.o"
	cd /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2/tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/gctest.dir/test.c.o   -c /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2/tests/test.c

tests/CMakeFiles/gctest.dir/test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gctest.dir/test.c.i"
	cd /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2/tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2/tests/test.c > CMakeFiles/gctest.dir/test.c.i

tests/CMakeFiles/gctest.dir/test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gctest.dir/test.c.s"
	cd /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2/tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2/tests/test.c -o CMakeFiles/gctest.dir/test.c.s

tests/CMakeFiles/gctest.dir/test.c.o.requires:

.PHONY : tests/CMakeFiles/gctest.dir/test.c.o.requires

tests/CMakeFiles/gctest.dir/test.c.o.provides: tests/CMakeFiles/gctest.dir/test.c.o.requires
	$(MAKE) -f tests/CMakeFiles/gctest.dir/build.make tests/CMakeFiles/gctest.dir/test.c.o.provides.build
.PHONY : tests/CMakeFiles/gctest.dir/test.c.o.provides

tests/CMakeFiles/gctest.dir/test.c.o.provides.build: tests/CMakeFiles/gctest.dir/test.c.o


# Object files for target gctest
gctest_OBJECTS = \
"CMakeFiles/gctest.dir/test.c.o"

# External object files for target gctest
gctest_EXTERNAL_OBJECTS =

tests/gctest: tests/CMakeFiles/gctest.dir/test.c.o
tests/gctest: tests/CMakeFiles/gctest.dir/build.make
tests/gctest: libgc-lib.a
tests/gctest: tests/CMakeFiles/gctest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable gctest"
	cd /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gctest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/gctest.dir/build: tests/gctest

.PHONY : tests/CMakeFiles/gctest.dir/build

tests/CMakeFiles/gctest.dir/requires: tests/CMakeFiles/gctest.dir/test.c.o.requires

.PHONY : tests/CMakeFiles/gctest.dir/requires

tests/CMakeFiles/gctest.dir/clean:
	cd /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2/tests && $(CMAKE_COMMAND) -P CMakeFiles/gctest.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/gctest.dir/clean

tests/CMakeFiles/gctest.dir/depend:
	cd /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2 /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2/tests /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2 /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2/tests /home/akshay/Colg_files/MCSV/work/CTLmc/gc-7.4.2/tests/CMakeFiles/gctest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/gctest.dir/depend

