include Config.mk

all: Pause

Pause.o: Pause.s
Pause: Pause.o

watch:
	while ! inotifywait -e modify *.s; do make; done

clean:
	$(RM) *.o Pause

.PHONY: all watch clean
