NAME=emulator
BUILD_DIR=build

default: release

#################
# Utility targets
#################

.PHONY: build-dir
build-dir:
	@mkdir -p $(BUILD_DIR)

.PHONY: compile
compile:
	@cd $(BUILD_DIR) && make

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)

.PHONY: tags
tags:
	@ctags .

.PHONY: format
format:
	@./etc/scripts/clang-format

.PHONY: tidy
tidy: debug
	@./etc/scripts/clang-tidy

###################
# Commands to build
###################

.PHONY: cmake
cmake-debug: build-dir
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=on ..

.PHONY: cmake-release
cmake-release: build-dir
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release ..

.PHONY: debug
debug: tags cmake-debug compile

.PHONY: release
release: tags cmake-release compile
