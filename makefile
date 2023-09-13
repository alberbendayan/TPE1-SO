all: 
	gcc -c shared_memory.h
	gcc -Wall -c child.c main.c view.c shared_memory.c
	gcc main.o shared_memory.o -o prog 
	gcc child.o -o child
	gcc view.o shared_memory.o -o view
	rm *.o
	
