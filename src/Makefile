.PHONY: clean all

CC = gcc

BASEDIR = ..
BUILDDIR = build
OUTDIR = $(BASEDIR)/out

CFLAGS = -g3 -O2 -Wfloat-equal -Wstrict-prototypes \
				 -Wno-unused-result

INCLUDE=-I$(BASEDIR)/include

LIB=-L$(BUILDDIR) -ldtlsr

_OBJ = graph/heap.o graph/local_graph.o graph/ls_graph.o \
			 graph/node.o graph/pathfind.o \
			 heartbeat/main.o \
			 network/packetsend.o \
			 process/core_node.o process/daemonise.o process/fd_event.o \
			 process/fileio.o process/logging.o
OBJ = $(patsubst %,$(BUILDDIR)/%,$(_OBJ))

_HEARTBEATOBJ = heartbeat/main.o
HEARTBEATOBJ = $(patsubst %,$(BUILDDIR)/%,$(_HEARTBEATOBJ))

_GRAPHOBJ = graph/main.o
GRAPHOBJ = $(patsubst %,$(BUILDDIR)/%,$(_GRAPHOBJ))

all: $(BUILDDIR) $(BUILDDIR)/libdtlsr.a heartbeat graph

$(BUILDDIR)/libdtlsr.a: $(OBJ)
	ar -rc $(BUILDDIR)/libdtlsr.a $(OBJ)
	ranlib $(BUILDDIR)/libdtlsr.a

heartbeat: $(HEARTBEATOBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(OUTDIR)/$@ $^ $(LIB)

graph: $(GRAPHOBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(OUTDIR)/$@ $^ $(LIB)

$(BUILDDIR)/%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)/graph
	mkdir -p $(BUILDDIR)/heartbeat
	mkdir -p $(BUILDDIR)/network
	mkdir -p $(BUILDDIR)/process

clean:
	rm -rf $(BUILDDIR)
