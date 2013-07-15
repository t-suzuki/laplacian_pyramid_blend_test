LDLIBS+=$(shell pkg-config opencv --libs)
CXXFLAGS+=$(shell pkg-config opencv --cflags)
CXXFLAGS+=$(shell pkg-config eigen3 --cflags)
CXXFLAGS+=-O3 -mfpmath=sse -march=core2 -msse4a -fopenmp
CXXFLAGS+=-g -Wall
CXX=g++

all: lp_blend
clean:
	rm -f *.o lp_blend

.cpp.o:
	$(CXX) -c $^ $(CXXFLAGS)

lp_blend: lp_blend.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

