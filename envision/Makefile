all:
	@echo "-- >> Attempting to compile default target [ $@ ]..."
	@if [ ! -r build/Makefile ]; then \
		echo "-- >> Build dir not yet initialized..."; \
		make init; \
	fi
	@cd build && make $@
	@echo "-- << Done attempting to compile default target"

init:
	@echo "-- >> Initializing build..."
	@mkdir -p build
	@cd build && cmake ..
	@echo "-- << Done initializing build"

remake:
	@make init && make

force_remake:
	@echo "-- >> Rebuilding project..."
	@make clean && make
	@echo "-- << Done rebuilding project"

debclean:
	@-rm -rf build

proprietary:
	@echo "-- >> Removing any proprietary content..."
	@-cd build && make $@
	@echo "-- << Done removing any proprietary content"

debinstall:
	@touch ROS_NOBUILD
	@touch installed

distclean: clean
	@echo "Removing any binary files"
	@-rm -rf lib
	@-rm -rf bin

test:
	@make test-results

install:
	@-cd build && make $@
	@make proprietary
	@make debclean
	@make debinstall

#forward all other commands, calling 'init' first if necessary
%:
	@echo "-- >> Forwarding target [ $@ ] to ./build ..."
	@if [ ! -r build ]; then \
		make init; \
	fi
	@cd build && make $@
	@echo "-- << Done forwarding target [ $@ ]"

clean:
	@echo "-- >> Cleaning project..."
	@-if [ ! -r build ]; then make init; fi
	@-cd build && make clean
	@-rm -rf build
	@echo "-- << Done cleaning project"
