# bench: bench.cc
# 	g++ -std=c++11 $^ -o $@ -lpthread

# clean:
# 	rm -f bench





CXX=g++
CXXFLAGS=-std=c++11 -g -O0 -fno-omit-frame-pointer
LDFLAGS=-lpthread

bench: bench.cc
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f bench