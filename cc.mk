include mk/config-cc.mk

all: Jit

Jit: Jit.o

watch:
	while ! inotifywait -e modify *.cc; do make; done

clean:
	$(RM) *.o Jit

.PHONY: all watch clean
