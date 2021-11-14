.PHONY: clean all

CC = gcc

BASEDIR = ..
BUILDDIR = build

CFLAGS = -g3 -O2 -Wfloat-equal -Wstrict-prototypes \
				 -Wno-unused-result

INCLUDE=-I$(BASEDIR)/utils

_OBJ = main.o
OBJ = $(patsubst %,$(BUILDDIR)/%,$(_OBJ))

all: $(BUILDDIR) arp-monitor

arp-monitor: $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BUILDDIR)/$@ $^

$(BUILDDIR)/%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
