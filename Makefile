
.PHONY: all clean clientserver clientserverclean arp arpclean hello helloclean


all: clientserver arp hello

clean: clientserverclean arpclean helloclean


clientserver:
	$(MAKE) -C clientserver all

clientserverclean:
	$(MAKE) -C clientserver clean

arp:
	$(MAKE) -C arp all

arpclean:
	$(MAKE) -C arp clean

hello:
	$(MAKE) -C hello all

helloclean:
	$(MAKE) -C hello clean
