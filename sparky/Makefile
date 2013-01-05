CMAKE_FLAGS=-D CMAKE_BUILD_TYPE=RELEASE $$BASH_CMAKE_FLAGS
MAKE_FLAGS=-j$$(cat /proc/cpuinfo | grep -c processor)

# make sure we default to all
all: init_build
	@echo "--> Building all targets..."
	@cd build && make $(MAKE_FLAGS)
	@echo "--< Done building all targets."

check_build:
	@echo "--> Checking if any build info exists..."
	@echo "cmake flags: $(CMAKE_FLAGS)"
	@echo "bash cmake flags: $$BASH_CMAKE_FLAGS"
	@if [ -r build ]; then echo "--< Build info exists."; else echo "--< No build info exists."; exit 1; fi

#check_build_full: check_build
check_build_full:
	@echo "--> Checking if build is working..."
	@if ( make check_build --quiet && cd build && make $(MAKE_FLAGS) --dry-run ); then echo "--< Build is working."; else echo "--< Build is not working."; exit 1; fi

# setup cmake
#any:
#	@mkdir -p build
#	cd build && cmake $(CMAKE_FLAGS) ..

init_build:
	@echo "--> Initializing build..."
	@if [ ! -r build ]; then echo "---   Creating build folder..."; mkdir -p build; fi
	@if ( ! make check_build_full ); then echo "---   Running cmake..."; cd build && cmake $(CMAKE_FLAGS) ..; fi
	@echo "--< Done initializing build."

#remake: init_build
remake:
	@make init_build && make $(MAKE_FLAGS)

force_remake: clean
	@make $(MAKE_FLAGS)

distclean: clean

clean:
	@echo "--> Cleaning..."
	@-if ( make check_build ); then cd build && make clean $(MAKE_FLAGS); fi
	-rm -rf build
	@echo "--< Done cleaning."

#test: check_build_full
#	@if ( make check_build ); then cd build && make test; else make all && make test; fi

#forward all other commands, calling 'init' first if necessary
%:
	@echo "--> Forwarding target $@..."
	@make init_build && cd build && make $@ $(MAKE_FLAGS)
	@echo "--< Done forwarding target $@."
