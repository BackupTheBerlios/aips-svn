# CMAKE generated file: DO NOT EDIT!
# Generated by "KDevelop3" Generator, CMake Version 2.2

# Rule file for object file evaltool.dir/cmeandistance.o.

src/evaltool.dir/cmeandistance.o.depend: src/cmeandistance.cpp
	@echo "Scanning CXX dependencies of src/evaltool.dir/cmeandistance.o"
	$(CMAKE_COMMAND) -E cmake_depends  "KDevelop3" /home/belitz/work/trunk/evaltool /home/belitz/work/trunk/evaltool/src CXX src/evaltool.dir/cmeandistance.o src/cmeandistance.cpp

# Include any dependencies generated for this rule.
include src/evaltool.dir/cmeandistance.o.build.depend.make

src/evaltool.dir/cmeandistance.o: src/cmeandistance.cpp
src/evaltool.dir/cmeandistance.o: src/evaltool.dir/cmeandistance.o.build.make
	@echo "Building CXX object src/evaltool.dir/cmeandistance.o"
	c++ -o src/evaltool.dir/cmeandistance.o -DDL3 -funroll-loops -fmove-all-movables -fprefetch-loop-arrays -mcpu=pentium4 -march=pentium4 -mfpmath=sse -O3 -I/home/belitz/include/aipsbase -I/usr/include/blitz -I/usr/include/libxml++-2.6 -I\ /usr/include/libxml2/libxml -I/usr/include/glibmm-2.4 -I/usr/lib/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/home/belitz/include/aipsfilehandlers   -Wall -c /home/belitz/work/trunk/evaltool/src/cmeandistance.cpp

src/evaltool.dir/cmeandistance.o.requires: src/evaltool.dir/cmeandistance.o.depend

src/evaltool.dir/cmeandistance.o.provides: src/evaltool.dir/cmeandistance.o.requires
	$(MAKE) -f src/evaltool.dir/build.make src/evaltool.dir/cmeandistance.o.provides.build

src/evaltool.dir/cmeandistance.o.provides.build: src/evaltool.dir/cmeandistance.o

