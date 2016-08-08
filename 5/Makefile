CFLAGS := -Wall -Wextra -g -ansi
OBJECTS := main.o mystring.o object.o

all: main

object.o: object.c object.h
	gcc -c object.c $(CFLAGS)

mystring.o: mystring.c mystring.h object.h
	gcc -c mystring.c $(CFLAGS)

main.o: main.c mystring.h object.h
	gcc -c main.c $(CFLAGS)

main: $(OBJECTS)
	gcc -o main $(OBJECTS) $(CFLAGS)

clean:
	rm -f $(OBJECTS) ./main
