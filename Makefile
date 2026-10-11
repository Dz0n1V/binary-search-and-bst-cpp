CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic

BUILD := build
TARGETS := $(BUILD)/task1_temperature_monitoring $(BUILD)/task2_hotel_loyalty_bst

.PHONY: all test clean

all: $(TARGETS)

$(BUILD)/%: src/%.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

# Builds with AddressSanitizer + UBSan and runs both test suites.
test:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="-std=c++17 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined" all
	python3 tests/test_task1.py
	python3 tests/test_task2.py

clean:
	rm -rf $(BUILD)
