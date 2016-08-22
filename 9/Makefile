CFLAGS := -Wall -Wextra -g -ansi
OBJECTS := main.o object.o mheap.o integer.o cons.o

all: main

object.o: object.c object.h integer.h mheap.h system_export.h
	gcc -c object.c $(CFLAGS)

mheap.o: mheap.c mheap.h object.h system_export.h
	gcc -c mheap.c $(CFLAGS)

integer.o: integer.c integer.h system_export.h
	gcc -c integer.c $(CFLAGS)

cons.o: cons.c cons.h integer.h system_export.h
	gcc -c cons.c $(CFLAGS)

main.o: main.c integer.h system_export.h cons.h
	gcc -c main.c $(CFLAGS)

main: $(OBJECTS)
	gcc -o main $(OBJECTS) $(CFLAGS)

clean:
	rm -f $(OBJECTS) ./main
