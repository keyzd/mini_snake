CC = gcc
CFLAGS = -g -Wall -pedantic -Wextra
LDFLAGS = -lSDL2 -lSDL2_image -lm

PROGNAME = msnake
SRCMODULES = msnake.c
OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -c -o $@

msnake: $(OBJMODULES)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

run: $(PROGNAME)
	./msnake

clean:
	rm -f *.o $(PROGNAME)

