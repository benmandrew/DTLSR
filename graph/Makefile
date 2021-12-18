.PHONY: clean all

CC = gcc

BASEDIR = ..
BUILDDIR = build

CFLAGS = -g3 -O2 -Wfloat-equal -Wstrict-prototypes \
				 -Wno-unused-result

INCLUDE=-I$(BASEDIR)/utils

_HEARTBEATOBJ = heartbeat.o
HEARTBEATOBJ = $(patsubst %,$(BUILDDIR)/%,$(_HEARTBEATOBJ))

_GRAPHOBJ = graph.o
GRAPHOBJ = $(patsubst %,$(BUILDDIR)/%,$(_GRAPHOBJ))

all: $(BUILDDIR) heartbeat graph

heartbeat: $(HEARTBEATOBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BUILDDIR)/$@ $^

graph: $(GRAPHOBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BUILDDIR)/$@ $^

$(BUILDDIR)/%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
