CXX = g++
CXXFLAGS = -std=c++17 -O2 -pthread -Wall -Wextra
TARGET = perf_lite_example
SOURCES = main.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run
