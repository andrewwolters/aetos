# Source, Executable, Includes, Library Defines
INCL   = ../mod.h
SRC    = hello.c
OBJ    = $(SRC:.c=.o)
LIBS   = -lc -lpth
MOD    = hello.mod

# Compiler, Linker Defines
CC      = gcc
CFLAGS  = -Wall -g -O1 -fPIC -I/usr/local/include/pth
LIBPATH = -L. -L/usr/local/lib -L/usr/local/lib/pth
LDFLAGS = -shared $(LIBPATH) $(LIBS)
CFDEBUG = -ansi -pedantic -Wall -g -DDEBUG $(LDFLAGS)
RM      = rm -f

# Link all Object Files with external Libraries into Binaries
$(MOD): $(OBJ)
	$(CC) -o $(MOD) $(LDFLAGS) $(OBJ)

# Compile and Assemble C Source Files into Object Files
%.o: %.c
	$(CC) $(CFLAGS) -c $*.c

# Objects depend on these Libraries
$(OBJ): $(INCL)

# Create a gdb/dbx Capable Executable with DEBUG flags turned on
debug:
	$(CC) $(CFDEBUG) $(SRC)

# Clean Up Objects, Exectuables, Dumps out of source directory
clean:
	$(RM) $(OBJ) $(MOD) core a.out


