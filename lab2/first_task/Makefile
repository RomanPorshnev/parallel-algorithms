CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = first_task

SOURCES = main.cpp threadsafe_queue.cpp
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJECTS) $(TARGET)
   
