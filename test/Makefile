CXX = g++
CXXFLAGS = -std=c++17 -O2 -pthread -Wall -Wextra
TARGET = perf_lite_example
SOURCES = main.cpp
INCLUDE_DIRS = -I../

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run
