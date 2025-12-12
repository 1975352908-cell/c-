CXX = g++
CXXFLAGS = -Wall -g `mysql_config --cflags`
LDFLAGS  = `mysql_config --libs` -lpthread

TARGET = test_db
SRCS = test.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -g -o  $(TARGET) $(LDFLAGS) -lpthread

clean:
	rm -f $(TARGET)
