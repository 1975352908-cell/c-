CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LDFLAGS = -lpthread

all: server client

server: server.cc
	$(CXX) $(CXXFLAGS) -g -o $@ $< $(LDFLAGS)

client: client.cc
	$(CXX) $(CXXFLAGS) -g -o $@ $< $(LDFLAGS)

clean:
	rm -f server client *.log
	rm -rf remote_logs

.PHONY: all clean

