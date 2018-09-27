CXX=clang++
CXXFLAGS=-g -pedantic -std=c++17 -fno-exceptions
SOURCES=$(wildcard sources/*.cpp)
BUILDDIRS=bin/
BIN=bin/cheap8

.PHONY: all clean

all: $(BUILDDIRS) $(BIN)

clean:
	rm -rf $(BUILDDIRS)

$(BUILDDIRS):
	mkdir $@

$(BIN): $(SOURCES)
	$(CXX) -o $@ $^

