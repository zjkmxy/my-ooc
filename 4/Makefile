CFLAGS := -Wall -Wextra -g -ansi
OBJECTS := main.o basics.o queue.o object.o msgqueue.o

all: main

object.o: object.c object.h
	gcc -c object.c $(CFLAGS)

queue.o: queue.c queue.h object.h
	gcc -c queue.c $(CFLAGS)

basics.o: basics.c basics.h object.h
	gcc -c basics.c $(CFLAGS)

msgqueue.o: msgqueue.c queue.h basics.h object.h
	gcc -c msgqueue.c $(CFLAGS)

main.o: main.c msgqueue.h queue.h basics.h object.h
	gcc -c main.c $(CFLAGS)

main: $(OBJECTS)
	gcc -o main $(OBJECTS) $(CFLAGS)

clean:
	rm -f $(OBJECTS) ./main
