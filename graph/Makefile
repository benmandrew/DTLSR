.PHONY: clean all

CC = gcc

BASEDIR = ..
BUILDDIR = build

CFLAGS = -g3 -O2 -Wfloat-equal -Wstrict-prototypes \
				 -Wno-unused-result

INCLUDE=-I$(BASEDIR)/utils
LIB=-L$(BASEDIR)/utils/build -lutils

_HEARTBEATOBJ = heartbeat.o
HEARTBEATOBJ = $(patsubst %,$(BUILDDIR)/%,$(_HEARTBEATOBJ))

_GRAPHOBJ = local_graph.o ls_graph.o graph_main.o
GRAPHOBJ = $(patsubst %,$(BUILDDIR)/%,$(_GRAPHOBJ))

all: $(BUILDDIR) heartbeat graph

heartbeat: $(HEARTBEATOBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BUILDDIR)/$@ $^ $(LIB)

graph: $(GRAPHOBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BUILDDIR)/$@ $^ $(LIB)

$(BUILDDIR)/%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
