
.PHONY: all clean utils utilsclean graph graphclean

all: utils graph

clean: utilsclean graphclean


utils:
	$(MAKE) -C utils all

utilsclean:
	$(MAKE) -C utils clean

graph:
	$(MAKE) -C graph all

graphclean:
	$(MAKE) -C graph clean
