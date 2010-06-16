CC= gcc
CFLAGS= -lexpat -O2
OBJECTS= xmlscrambler
DBG_CFLAGS= -Wall -Werror -g
DBG_OBJECTS= xmlscrambler-test

.PHONY: all clean debug
all: $(OBJECTS)
	strip $(OBJECTS)
debug: $(DBG_OBJECTS)
clean:
	rm -f $(OBJECTS) $(DBG_OBJECTS)

xmlscrambler-test: xmlscrambler.c
	$(CC) $(CFLAGS) $(DBG_CFLAGS) -o $@ $^

