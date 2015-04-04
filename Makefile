include Config.mk

all: Pause Exit

Pause: Pause.o
Exit: Exit.o

watch:
	while ! inotifywait -e modify *.s; do make; done

clean:
	$(RM) *.o Pause Exit

.PHONY: all watch clean
