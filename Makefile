include Config.mk

all: Hello

Hello.o: Hello.s
Hello: Hello.o

clean:
	$(RM) *.o Hello

.PHONY: all clean
