# bench: bench.cc
# 	g++ -std=c++11 $^ -o $@ -lpthread

# clean:
# 	rm -f bench





#编译选项加上mysql预编译头文件
CXX      := g++
TARGET   := bench
SRCS     := bench.cc
OBJS     := $(SRCS:.cc=.o)

MYSQL_CFLAGS := $(shell mysql_config --cflags)
MYSQL_LIBS   := $(shell mysql_config --libs)

CXXFLAGS := -std=c++17 -g -O0 -fno-omit-frame-pointer -Wall -Wextra $(MYSQL_CFLAGS)
LDLIBS   := $(MYSQL_LIBS)
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDLIBS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
