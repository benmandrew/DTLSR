
.PHONY: all clean

OUTDIR = out

all: $(OUTDIR)
	$(MAKE) -C src all

$(OUTDIR):
	mkdir -p $(OUTDIR)

clean:
	$(MAKE) -C src clean
	rm -rf $(OUTDIR)
