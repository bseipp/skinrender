
REQ_FILES = src/materials/skin.cpp src/materials/skin.h src/media/grid.h src/media/grid.cpp src/core/api.cpp src/core/floatfile.cpp

default: test_prereqs

test_prereqs: $(REQ_FILES)
	@echo "Good so far"

install: $(REQ_FILES)
	@mkdir src/build
	@cd src/build; cmake3 ../../;

clean:
	@rm -rf src/build
	@rm -rf build
	@echo "build environment cleaned"
