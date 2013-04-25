all: midterm

midterm: midterm.o
	gcc midterm.o -o midterm 

midterm.o:
	gcc -c midterm.c

clean:
	rm -rf *o midterm
