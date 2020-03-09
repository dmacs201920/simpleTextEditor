Exe: today.o
	gcc -o Exe today.o -lncurses
today.o: today.c
	gcc -c today.c -lncurses
clean:
	rm today.o Exe
