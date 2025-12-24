.PHONY:config_test config_usage all clean

all:config_test config_usage

config_test:config_test.cpp config.hpp level.hpp
	g++ -g -std=c++17 config_test.cpp -o config_test
	chmod +x config_test


config_usage:config_usage_example.cpp config.hpp level.hpp
	g++ -g -std=c++17 config_usage_example.cpp -o config_usage
	chmod +x config_usage
clean:
	rm -f config_test config_usage