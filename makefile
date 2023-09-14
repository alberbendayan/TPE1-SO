all: 
	gcc -c sharedMemory.h
	gcc -Wall -c child.c main.c view.c sharedMemory.c
	gcc main.o sharedMemory.o -o prog 
	gcc child.o -o child
	gcc view.o sharedMemory.o -o view
	rm *.o
	rm *.gch
	
