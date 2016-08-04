CFLAGS := -Wall -Wextra -g -ansi
OBJECTS := main.o array.o list.o sequence.o stream.o

all: main

stream.o: stream.c stream.h sequence.h
	gcc -c stream.c $(CFLAGS)

array.o: array.c array.h sequence.h
	gcc -c array.c $(CFLAGS)

list.o: list.c list.h sequence.h
	gcc -c list.c $(CFLAGS)

sequence.o: sequence.c sequence.h
	gcc -c sequence.c $(CFLAGS)

main.o: main.c list.h array.h sequence.h
	gcc -c main.c $(CFLAGS)

main: $(OBJECTS)
	gcc -o main $(OBJECTS) $(CFLAGS)

clean:
	rm -f $(OBJECTS) ./main
