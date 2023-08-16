# 

# Define SAN=1 to build with memory sanitizer.
ifeq ($(SAN),1)
  SANFLAGS = -g -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment
  LIBSAN = -lasan -lubsan 
endif

CXX = g++
CXXFLAGS = -Wall -O2 $(SANFLAGS)
OBJ = main.o 
HDR = 2d.h board.h mgr.h move.h piece.h soln.h solve.h worker.h

slider: $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBSAN) -lpthread 

main.o: $(HDR)

clean:
	-rm -rf slider *.o
