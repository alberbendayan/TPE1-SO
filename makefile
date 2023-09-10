all: 
	gcc -Wall -c child.c main.c view.c
	gcc main.o -o prog 
	gcc child.o -o child
	#gcc view.o -o view
	rm *.o
	
