BUILD_DIR ?= build

.PHONY: all configure clean

all: configure
	cmake --build $(BUILD_DIR) --parallel

configure:
	cmake -S . -B $(BUILD_DIR)

clean:
	cmake -E rm -rf $(BUILD_DIR)
