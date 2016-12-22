include makefile.define

default:all

PATS = runepool.o nodepool.o wordbase.o ShareMemory.o main.o

LIB_BASE_OBJS = runepool.o nodepool.o wordbase.o ShareMemory.o main.o

LIB_BASE = fenci

all: fenci

# ?????
all:$(LIB_BASE) makefile

$(LIB_BASE):$(PATS)
	$(CC) -rdynamic -o $(LIB_BASE) $(LIB_BASE_OBJS) $(LIBS)

# ??
clean:
	rm -rf *.o  $(LIB_BASE) $(LIB_BASE_OBJS)
cl:
	rm -rf *.o 
