# Makefile for OpenDTU-OnBattery tests

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TEST_OVERSCALING = test_overscaling
TEST_CHARGEDIST  = test_chargedistributor
TEST_VOLTFLOOR   = test_voltagefloor

.PHONY: all clean test help

all: $(TEST_OVERSCALING) $(TEST_CHARGEDIST) $(TEST_VOLTFLOOR)

$(TEST_OVERSCALING): test_overscaling.cpp ../src/OverscalingCalculator.cpp
	$(CXX) $(CXXFLAGS) -I../include -o $@ $< ../src/OverscalingCalculator.cpp

$(TEST_CHARGEDIST): test_chargedistributor.cpp ../src/solarcharger/ChargeCurrentDistributor.cpp
	$(CXX) $(CXXFLAGS) -I../include -o $@ $^

$(TEST_VOLTFLOOR): test_voltagefloor.cpp ../include/BatteryVoltageFloor.h
	$(CXX) $(CXXFLAGS) -I../include -o $@ $<

test: all
	@echo "Running overscaling tests..."
	./$(TEST_OVERSCALING)
	@echo "Running charge current distributor tests..."
	./$(TEST_CHARGEDIST)
	@echo "Running voltage floor tests..."
	./$(TEST_VOLTFLOOR)

clean:
	rm -f $(TEST_OVERSCALING) $(TEST_CHARGEDIST) $(TEST_VOLTFLOOR)

help:
	@echo "Available targets:"
	@echo "  all    - Build all test executables"
	@echo "  test   - Build and run all tests"
	@echo "  clean  - Remove test executables"
	@echo "  help   - Show this help"
