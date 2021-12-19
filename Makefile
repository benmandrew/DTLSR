
.PHONY: all clean utils utilsclean clientserver clientserverclean arp arpclean graph graphclean

all: utils clientserver arp graph

clean: utilsclean clientserverclean arpclean graphclean


utils:
	$(MAKE) -C utils all

utilsclean:
	$(MAKE) -C utils clean

clientserver:
	$(MAKE) -C clientserver all

clientserverclean:
	$(MAKE) -C clientserver clean

arp:
	$(MAKE) -C arp all

arpclean:
	$(MAKE) -C arp clean

graph:
	$(MAKE) -C graph all

graphclean:
	$(MAKE) -C graph clean
