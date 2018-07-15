CXX=clang++
CXXFLAGS=-Wall -std=c++11 -Wno-deprecated

TARGET=program
SOURCES=$(wildcard *.cpp)
SOURCES+=$(wildcard crc32/*.c)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) 

all: $(TARGET)
clean: 
	rm $(TARGET)