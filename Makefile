# Define SAN=1 to build with memory sanitizer.
# Define PROF=1 to build with gprof profiling.
SAN=
PROF=

CXX = g++
CXXFLAGS = -Wall -O2 $(SANFLAGS)
LIBS = -lpthread

ifeq ($(SAN),1)
  SANFLAGS = -g -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment
  LIBSAN += -lasan -lubsan 
endif
ifeq ($(PROF),1)
  CXXFLAGS += -pg
  LDFLAGS += -pg
endif

OBJ = main.o 
HDR = 2d.h board.h mgr.h move.h piece.h soln.h solve.h worker.h

slider: $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBSAN) $(LIBS)

main.o: $(HDR)

clean:
	-rm -rf slider *.o
