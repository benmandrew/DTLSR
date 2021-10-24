.PHONY: clean all

CC = gcc

# CFLAGS=

BUILDDIR = build

# _DEPS =
# DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))


_CLIENTOBJ = client.o
CLIENTOBJ = $(patsubst %,$(BUILDDIR)/%,$(_CLIENTOBJ))

_SERVEROBJ = server.o
SERVEROBJ = $(patsubst %,$(BUILDDIR)/%,$(_SERVEROBJ))

all: $(BUILDDIR) client server

client: $(CLIENTOBJ)
	$(CC) -o $(BUILDDIR)/$@ $^ $(CFLAGS)

server: $(SERVEROBJ)
	$(CC) -o $(BUILDDIR)/$@ $^ $(CFLAGS)

$(BUILDDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)