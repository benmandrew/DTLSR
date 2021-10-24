.PHONY: clean all

CC = gcc

# CFLAGS=

BUILDDIR = build

# _DEPS =
# DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))


_OBJ = main.o
OBJ = $(patsubst %,$(BUILDDIR)/%,$(_OBJ))

all: $(BUILDDIR) main

main: $(OBJ)
	$(CC) -o $(BUILDDIR)/$@ $^ $(CFLAGS)

$(BUILDDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)