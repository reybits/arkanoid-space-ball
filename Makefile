VER_MAJOR=1
VER_MINOR=3
VER_RELEASE=7
VERSION=$(VER_MAJOR).$(VER_MINOR)$(VER_RELEASE)

BUILD_DIR=.build
BUILD_DIR_RELEASE=${BUILD_DIR}/release
BUILD_DIR_DEBUG=${BUILD_DIR}/debug
BUILD_DIR_EMSCRIPTEN=${BUILD_DIR}/emscripten
CMAKE_DIR=../..

BUNDLE_NAME=arkanoidsb

PREFIX?=/usr/local

UNAME=$(shell uname -s)
ifeq ($(UNAME), Darwin)
	BUNDLE_NAME=arkanoidsb.app
endif

all:
	@echo "Usage:"
	@echo "    make <release | debug>    - make release or debug application"
	@echo "    make <emscripten>         - make release of web version"
	@echo "    make <cppcheck>           - do static code verification"
	@echo "    make <clean>              - cleanup directory"

package:
	cd tools/rescompiler && make && cd ../../res && ../tools/rescompiler/rescompiler arkanoidsb
	cp res/arkanoidsb.pak .

release: package
	$(shell if [ ! -d $(BUILD_DIR_RELEASE) ]; then mkdir -p $(BUILD_DIR_RELEASE); fi)
	cd $(BUILD_DIR_RELEASE) ; \
		cmake -DCMAKE_BUILD_TYPE=Release \
		-DAPP_VERSION_MAJOR:STRING=$(VER_MAJOR) \
		-DAPP_VERSION_MINOR:STRING=$(VER_MINOR) \
		-DAPP_VERSION_RELEASE:STRING=$(VER_RELEASE) ${CMAKE_DIR} ; \
		make
	cp -r $(BUILD_DIR_RELEASE)/$(BUNDLE_NAME) .

debug: package
	$(shell if [ ! -d $(BUILD_DIR_DEBUG) ]; then mkdir -p $(BUILD_DIR_DEBUG); fi)
	cd $(BUILD_DIR_DEBUG) ; \
		cmake -DCMAKE_BUILD_TYPE=Debug \
		-DAPP_VERSION_MAJOR:STRING=$(VER_MAJOR) \
		-DAPP_VERSION_MINOR:STRING=$(VER_MINOR) \
		-DAPP_VERSION_RELEASE:STRING=$(VER_RELEASE) ${CMAKE_DIR} ; \
		make
	cp -r $(BUILD_DIR_DEBUG)/$(BUNDLE_NAME) .

emscripten: package
	$(shell if [ ! -d $(BUILD_DIR_EMSCRIPTEN) ]; then mkdir -p $(BUILD_DIR_EMSCRIPTEN); fi)
	cd $(BUILD_DIR_EMSCRIPTEN) \
		&& emcmake cmake -DCMAKE_BUILD_TYPE=Release \
		-DAPP_VERSION_MAJOR:STRING=$(VER_MAJOR) \
		-DAPP_VERSION_MINOR:STRING=$(VER_MINOR) \
		-DAPP_VERSION_RELEASE:STRING=$(VER_RELEASE) ${CMAKE_DIR} \
		&& emmake make
	rm -fr html && mkdir html \
		&& cp $(BUILD_DIR_EMSCRIPTEN)/arkanoidsb.* html/ \
		&& mv html/arkanoidsb.html html/index.html

cppcheck:
	cppcheck -j 1 --enable=all -f -I src src/ 2> cppcheck-output

clean:
	cd tools/rescompiler && make clean
	rm -fr $(BUILD_DIR) $(BUNDLE_NAME) html/ cppcheck-output \
		$(BUNDLE_NAME)-$(VERSION)* \
		$(BUNDLE_NAME)_$(VERSION)* \
		*.{log,tasks,sh,xz,list} \
		strace_out cov-int
