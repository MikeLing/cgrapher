CXXFLAGS := -fPIC -O2 -std=c++11
INC := -I/usr/lib/llvm-3.9/include
SRCS = cgrapher.cpp

TMPDIR = /tmp
LOCK_FILE = $(TMPDIR)/mylock
RES_FILE = $(TMPDIR)/result.txt
ROOT = `pwd`/

CLANGPP = clang++-3.9
CLANGPP_FLAGS = -std=c++11 -fsyntax-only
CLANGPP_OPTS = -fplugin=./libprotofun.so -Xclang -plugin-arg-protofun -Xclang $(ROOT) -Xclang -plugin-arg-protofun -Xclang $(RES_FILE) -Xclang -plugin-arg-protofun -Xclang $(LOCK_FILE)

build: cgrapher.so

cgrapher.o: $(SRCS)
		$(CXX) $(CXXFLAGS) $(INC) -c $^ -o $@

cgrapher.so: cgrapher.o
		$(CXX) -shared $^ -o $@

clean:
		$(RM) cgrapher.so *.o


.PHONY: build clean
