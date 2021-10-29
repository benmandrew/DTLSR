.PHONY: clean all

CC = gcc

# CFLAGS=

BUILDDIR = build

CFLAGS = -g3 -O2 -Wfloat-equal -Wstrict-prototypes -Wno-unused-result

# _DEPS =
# DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))


_CLIENTOBJ = client.o
CLIENTOBJ = $(patsubst %,$(BUILDDIR)/%,$(_CLIENTOBJ))

_SERVEROBJ = server.o
SERVEROBJ = $(patsubst %,$(BUILDDIR)/%,$(_SERVEROBJ))

all: $(BUILDDIR) client server

client: $(CLIENTOBJ)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $^

server: $(SERVEROBJ)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $^

$(BUILDDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)