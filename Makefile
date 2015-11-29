CC=g++
CXX=g++
CXXFLAGS=-pedantic -W -Wall -std=c++11 -O2 -DNDEBUG
LDFLAGS=-O2

MAIN=gen_complete gen_complete_tpartite gen_random gen_circulant gen_hypercube solver solver-old solver-bad solver-quick

HEADERS=loader.h graph.h

all: $(MAIN)

clean:
	rm -rf *.o $(MAIN)

solver.o: solver.cc $(HEADERS)
solver-old.o: solver-old.cc $(HEADERS)
solver-bad.o: solver-bad.cc $(HEADERS)
solver-quick.o: solver-quick.cc $(HEADERS)

gen_complete: gen_complete.o
gen_complete_tpartite: gen_complete_tpartite.o
gen_random: gen_random.o
gen_circulant: gen_circulant.o
gen_hypercube: gen_hypercube.o
solver: solver.o loader.o 
solver-old: solver-old.o loader.o
solver-bad: solver-bad.o loader.o
solver-quick: solver-quick.o loader.o

