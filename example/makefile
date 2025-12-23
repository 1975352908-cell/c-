CXX      = g++
CXXFLAGS = -Wall -Wextra -g -std=c++17 $(shell mysql_config --cflags)
LDFLAGS  = $(shell mysql_config --libs) -lpthread

TARGET = test
SRCS   = test.cc

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
