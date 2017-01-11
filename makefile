include makefile.define

default:all

PATS = nodepool.o wordbase.o RuneHMMInfo.o HMMDict.o WordDict.o ShareMemory.o TextRank.o main.o

LIB_BASE_OBJS = nodepool.o wordbase.o RuneHMMInfo.o HMMDict.o WordDict.o ShareMemory.o TextRank.o main.o

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
