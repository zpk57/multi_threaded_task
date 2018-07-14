CXX=clang++
CXXFLAGS=-Wall -std=c++11

TARGET=program
SOURCES=main.cpp ThreadPool.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) 

all: $(TARGET)
clean: 
	rm $(TARGET)