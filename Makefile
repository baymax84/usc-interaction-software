# set EXTRA_CMAKE_FLAGS in the including Makefile in order to add tweaks
CMAKE_FLAGS= -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr
MAKE_FLAGS=-j$$(cat /proc/cpuinfo | grep -c processor)
#NOBUILD := $(shell if [ -r ROS_NOBUILD ]; then echo 1; else echo 0; fi)

all:
	@echo "-- >> Attempting to compile default target [ $@ $(MAKE_FLAGS) ]..."
	@if [ ! -r build/Makefile ]; then \
		echo "-- >> Build dir not yet initialized..."; \
		make init; \
	fi
	@cd build && make $@ $(MAKE_FLAGS)
	@echo "-- << Done attempting to compile default target"

init:
	@echo "-- >> Initializing build..."
	@mkdir -p build
	@cd build && cmake $(CMAKE_FLAGS) ..
	@echo "-- << Done initializing build"

remake:
	@make init && make

force_remake:
	@echo "-- >> Rebuilding project..."
	@make clean && make
	@echo "-- << Done rebuilding project"

distclean: clean
	@echo "Removing any binary files"
	@-rm -rf build

test:
	@echo "-- >> Building tests, if any..."
	@-cd build && make $@ $(MAKE_FLAGS)
	@echo "-- << Done building tests"

#forward all other commands, calling 'init' first if necessary
%:
	@echo "-- >> Forwarding target [ $@ $(MAKE_FLAGS) ] to ./build ..."
	@if [ ! -r build ]; then \
		make init; \
	fi
	@cd build && make $@ $(MAKE_FLAGS)
	@echo "-- << Done forwarding target [ $@ ]"

clean:
	@echo "-- >> Cleaning project..."
	@-if [ ! -r build ]; then make init; fi
	@-cd build && make clean
	@echo "-- << Done cleaning project"
