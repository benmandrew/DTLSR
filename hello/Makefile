.PHONY: clean all debug

CC = gcc

BASEDIR = ..
BUILDDIR = build

CFLAGS = -g3 -O2 -Wfloat-equal -Wstrict-prototypes \
				 -Wno-unused-result

# CFLAGS = -DDEBUG -g3 -O2 -Wfloat-equal -Wstrict-prototypes \
# 				 -Wno-unused-result

INCLUDE=-I$(BASEDIR)/logging

# _DEPS =
# DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))


_CLIENTOBJ = client.o
CLIENTOBJ = $(patsubst %,$(BUILDDIR)/%,$(_CLIENTOBJ))

_SERVEROBJ = server.o
SERVEROBJ = $(patsubst %,$(BUILDDIR)/%,$(_SERVEROBJ))

all: $(BUILDDIR) client server

client: $(CLIENTOBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BUILDDIR)/$@ $^

server: $(SERVEROBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BUILDDIR)/$@ $^

$(BUILDDIR)/%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)