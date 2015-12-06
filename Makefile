CC=g++
CXX=g++
CXXFLAGS=-pedantic -W -Wall -std=c++11 -O2 -DNDEBUG
LDFLAGS=-O2

MAIN=gen_complete gen_complete_tpartite gen_random gen_circulant gen_hypercube solver 

HEADERS=loader.h graph.h bestfound.h tools.h

all: $(MAIN)

clean:
	rm -rf *.o $(MAIN)

solver.o: solver.cc $(HEADERS)
bestfound.o: bestfound.cc $(HEADERS)
loader.o: loader.cc $(HEADERS)
tools.o: tools.cc $(HEADERS)

gen_complete: gen_complete.o
gen_complete_tpartite: gen_complete_tpartite.o
gen_random: gen_random.o
gen_circulant: gen_circulant.o
gen_hypercube: gen_hypercube.o
solver: solver.o loader.o bestfound.o tools.o 
