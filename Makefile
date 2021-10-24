
.PHONY: all clean hello helloclean


all: hello

clean: helloclean


hello:
	$(MAKE) -C hello all

helloclean:
	$(MAKE) -C hello clean
