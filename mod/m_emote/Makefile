# Source, Executable, Includes, Library Defines
INCL   = ../mod.h
SRC    = emote.c
OBJ    = $(SRC:.c=.o)
LIBS   = -lc -lpq
MOD    = emote.mod

# Compiler, Linker Defines
CC      = gcc
CFLAGS  = -g -Wall -O1 -fPIC -I../../.. -I/usr/local/include
LIBPATH = -L. -L/usr/local/lib
LDFLAGS = -shared $(LIBPATH) $(LIBS)
CFDEBUG = -ansi -pedantic -Wall -g -DDEBUG $(LDFLAGS)
RM      = rm -f

# Compile and Assemble C Source Files into Object Files
%.o: %.c
	$(CC) $(CFLAGS) -c $*.c

# Link all Object Files with external Libraries into Binaries
$(MOD): $(OBJ)
	$(CC) -o $(MOD) $(LDFLAGS) $(OBJ)

# Objects depend on these Libraries
$(OBJ): $(INCL)

# Create a gdb/dbx Capable Executable with DEBUG flags turned on
debug:
	$(CC) $(CFDEBUG) $(SRC)

# Clean Up Objects, Exectuables, Dumps out of source directory
clean:
	$(RM) $(OBJ) $(MOD) core a.out


