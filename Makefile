SOURCEDIR:=./src
SOURCES:=$(shell find ${SOURCEDIR} -name '*.cpp') $(shell find ${SOURCEDIR} -name '*.h')

BUILDDIR=./build

.PHONY: config
config:
	mkdir -p ${BUILDDIR}
	cd ${BUILDDIR} && \
		cmake -GNinja ..

.PHONY: build
build:
	cd ${BUILDDIR} && \
		cmake --build . --config Debug -- -j10

.PHONY: test
test:
	cd ${BUILDDIR} && \
		ctest -j10 -C Debug -T test --output-on-failure

.PHONY: clean
clean:
	rm -rf ${BUILDDIR}

format:
	@clang-format -style=chromium -i $(SOURCES)

tidy:
	@clang-tidy $(SOURCES) -p ./build
