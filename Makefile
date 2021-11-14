
.PHONY: all clean hello helloclean arp arpclean


all: hello arp

clean: helloclean arpclean


hello:
	$(MAKE) -C hello all

helloclean:
	$(MAKE) -C hello clean

arp:
	$(MAKE) -C arp all

arpclean:
	$(MAKE) -C arp clean