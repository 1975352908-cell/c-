CXX = g++
CXXFLAGS = -Wall -g `mysql_config --cflags`
LDFLAGS  = `mysql_config --libs`

TARGET = test_db
SRCS = test.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -g -o  $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
