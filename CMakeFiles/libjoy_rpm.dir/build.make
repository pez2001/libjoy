# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.4

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pez2001/libjoy-dev/libjoy-0.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pez2001/libjoy-dev/libjoy-0.1

# Utility rule file for libjoy_rpm.

CMakeFiles/libjoy_rpm:
	cpack -G TGZ --config CPackSourceConfig.cmake
	/usr/bin/cmake -E copy libjoy-0.1.1-Source.tar.gz /home/pez2001/libjoy-dev/libjoy-0.1/RPM/SOURCES
	/usr/bin/rpmbuild -bb --define="_topdir /home/pez2001/libjoy-dev/libjoy-0.1/RPM" --buildroot=/home/pez2001/libjoy-dev/libjoy-0.1/RPM/tmp /home/pez2001/libjoy-dev/libjoy-0.1/RPM/SPECS/libjoy.spec

libjoy_rpm: CMakeFiles/libjoy_rpm.dir/build.make

# Rule to build all files generated by this target.
CMakeFiles/libjoy_rpm.dir/build: libjoy_rpm
CMakeFiles/libjoy_rpm.dir/build: CMakeFiles/libjoy_rpm

CMakeFiles/libjoy_rpm.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/libjoy_rpm.dir/cmake_clean.cmake

