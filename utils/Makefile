.PHONY: clean all

CC = gcc

BASEDIR = ..
BUILDDIR = build

CFLAGS = -g3 -O2 -Wfloat-equal -Wstrict-prototypes \
				 -Wno-unused-result

INCLUDE=-I$(BASEDIR)/utils

_OBJ = arp.o daemonise.o fd_event.o fileio.o logging.o packetsend.o node.o core_node.o
OBJ = $(patsubst %,$(BUILDDIR)/%,$(_OBJ))

all: $(BUILDDIR) $(OBJ)
	ar -rc $(BUILDDIR)/libutils.a $(OBJ)
	ranlib $(BUILDDIR)/libutils.a

$(BUILDDIR)/%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
