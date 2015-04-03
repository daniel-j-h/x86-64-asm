include Config.mk

all: Pause

Pause.o: Pause.s
Pause: Pause.o

clean:
	$(RM) *.o Pause

.PHONY: all clean
