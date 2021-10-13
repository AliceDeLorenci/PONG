# C++ compiler	
CC=g++		# change g++ to gcc when using C!	

DEFS=

CFLAGS= $(DEFS) -g -Wall -std=c++17

# executable name
EXEC= pong

# LIBS=-lm
LIBS = -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17

# headers directory
HDIR = header
# source code directory
CDIR = src
# build directory ( where the object files will be stored )
ODIR = build

# list of header files
_DEPS = olcPixelGameEngine.h Player.h Ball.h Pong.h 
DEPS = $(patsubst %,$(HDIR)/%,$(_DEPS))

# list of source code files ( replacing the .c/.cpp extension with .o )
_OBJ = olcPixelGameEngine.o Player.o Ball.o Pong.o main.o 
OBJ = $(patsubst %, $(ODIR)/%,$(_OBJ))

# used to create the build directory if it doesn't exist
dir_guard = @mkdir -p $(@D)

# rule for files ending in .o
$(ODIR)/%.o: $(CDIR)/%.cpp $(DEPS)	# change cpp to c when using C!
	$(dir_guard)
	$(CC) -c -o $@ $< $(CFLAGS)

# "make all" to compile
.PHONY: all
all: $(OBJ)
	$(CC) -o $(EXEC) $^ $(CFLAGS) $(LIBS)

# "make run" to execute
.PHONY: run
run:
	./$(EXEC)

# "make debug" to execute with debugger
.PHONY: debug
debug:
	gdb ./$(EXEC)

# "make valgrind" to execute with valgrind (memory leak detection)
.PHONY: valgrind
valgrind:
	valgrind --leak-check=full ./$(EXEC)

# "make clean" to remove object files
.PHONY: clean
clean:
	rm -f $(ODIR)/*.o  
