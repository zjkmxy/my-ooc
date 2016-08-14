CFLAGS := -Wall -Wextra -g -ansi
OBJECTS := main.o amb.o

all: main

amb.o: amb.c amb.h
	gcc -c amb.c $(CFLAGS)

main.o: main.c amb.h
	gcc -c main.c $(CFLAGS)

main: $(OBJECTS)
	gcc -o main $(OBJECTS) $(CFLAGS)

clean:
	rm -f $(OBJECTS) ./main
