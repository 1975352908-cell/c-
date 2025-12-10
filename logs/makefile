.PHONY:test
# 正确的makefile（适配x86_64 Ubuntu）
test: test.cc format.hpp util.hpp level.hpp message.hpp
    # 仅指定-std=c++13，不添加-m32/-m64等架构参数（默认适配当前系统）
	g++ -std=c++17 $^ -o $@
	chmod +x $@  # 自动添加执行权限

clean:
	rm -f test