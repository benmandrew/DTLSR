.PHONY: clean all

CC = gcc

BASEDIR = ..
BUILDDIR = build

CFLAGS = -g3 -O2 -Wfloat-equal -Wstrict-prototypes \
				 -Wno-unused-result

INCLUDE=-I$(BASEDIR)/utils

_HBOBJ = heartbeat.o
HBOBJ = $(patsubst %,$(BUILDDIR)/%,$(_CLIENTOBJ))

_SERVEROBJ = server.o
SERVEROBJ = $(patsubst %,$(BUILDDIR)/%,$(_SERVEROBJ))

all: $(BUILDDIR) heartbeat server

heartbeat: $(CLIENTOBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BUILDDIR)/$@ $^

server: $(SERVEROBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BUILDDIR)/$@ $^

$(BUILDDIR)/%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
