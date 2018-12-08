###################################
# OS COURSE - 2006
# Michael Lingo and Shane Laskowski
###################################
CC = g++                        # use g++ for compiling c++ code or gcc for c code
CFLAGS = -O3 -Wall -std=c++14              # compilation flags: -g for debugging. Change to -O or -O2 for optimized code.
LIB = -lm                       # linked libraries      
LDFLAGS = -L. -flto                  # link flags
PROG = a.out                   # target executable (output)
SRC = page.cpp main.cpp   #.cpp source files.
OBJ = $(SRC:.cpp=.o)    # object files for the target. Add more to this and next lines if there are more than one source files.

all : $(PROG)

$(PROG): $(OBJ) 
				$(CC) -o $(PROG) $(OBJ) $(LDFLAGS) $(LIB)

.cpp.o:
				$(CC) -c $(CFLAGS) $< -o $@

# cleanup
clean:
				rm -f *.o $(PROG)

# DO NOT DELETE
