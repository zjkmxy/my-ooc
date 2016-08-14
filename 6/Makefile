CFLAGS := -Wall -Wextra -g -ansi
OBJECTS := main.o object.o

all: main

object.o: object.c object.h
	gcc -c object.c $(CFLAGS)

main.o: main.c object.h
	gcc -c main.c $(CFLAGS)

main: $(OBJECTS)
	gcc -o main $(OBJECTS) $(CFLAGS)

clean:
	rm -f $(OBJECTS) ./main
