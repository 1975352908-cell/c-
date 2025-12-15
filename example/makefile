CXX = g++
CXXFLAGS = -Wall -g
LDFLAGS  = -lpthread

TARGET = test
SRCS = test.cc

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -g -o  $(TARGET) $(LDFLAGS) -lpthread

clean:
	rm -f $(TARGET)
	g++ -std=c++17 $^ -o $@
	chmod +x $@  # 自动添加执行权限

clean:
	rm -f test