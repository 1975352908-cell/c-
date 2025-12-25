# 基础编译选项 (改用 CXXFLAGS，C++标准变量)
CXXFLAGS = -g -Wall

# MySQL 编译选项
MYSQL_CFLAGS = -I/usr/include/mysql

# MySQL 链接选项
MYSQL_LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lz -lzstd -lssl -lcrypto -lresolv -lm

TARGET = test
SRC = test.cc
OBJ = $(patsubst %.cc,%.o,$(SRC))

all: $(TARGET)

# 链接步骤
$(TARGET): $(OBJ)
	g++ -g -o $@ $^ -lpthread $(MYSQL_LDFLAGS)

# 编译步骤：这里改成了 %.cc，确保能匹配到你的 test.cc
# 并且把 CFLAGS 换成了 CXXFLAGS
%.o: %.cc
	g++ -c $< $(CXXFLAGS) $(MYSQL_CFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)