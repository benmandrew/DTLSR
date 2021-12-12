# .PHONY: clean all

# CC = gcc

# BASEDIR = ..
# BUILDDIR = build

# CFLAGS = -g3 -O2 -Wfloat-equal -Wstrict-prototypes \
# 				 -Wno-unused-result

# LINKFLAGS = -L/usr/local/lib -l:libev.a

# INCLUDE=-I$(BASEDIR)/utils

# _OBJ = main.o
# OBJ = $(patsubst %,$(BUILDDIR)/%,$(_OBJ))

# all: $(BUILDDIR) event

# event: $(OBJ)
# 	$(CC) $(CFLAGS) $(INCLUDE) $(LINKFLAGS) -o $(BUILDDIR)/$@ $^

# $(BUILDDIR)/%.o: %.c $(DEPS)
# 	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

# $(BUILDDIR):
# 	mkdir -p $(BUILDDIR)

# clean:
# 	rm -rf $(BUILDDIR)


.PHONY: clean all

CC = gcc

BASEDIR = ..
BUILDDIR = build

CFLAGS = -g3 -O2 -Wfloat-equal -Wstrict-prototypes \
				 -Wno-unused-result

INCLUDE=-I$(BASEDIR)/utils

_OBJ = main.o
OBJ = $(patsubst %,$(BUILDDIR)/%,$(_OBJ))

all: $(BUILDDIR) event

event: $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BUILDDIR)/$@ $^

$(BUILDDIR)/%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

