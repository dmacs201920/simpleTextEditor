Exe: Editor.o
	gcc -o Exe Editor.o -lncurses
Editor.o: Editor.c
	gcc -c Editor.c -lncurses
clean:
	rm Editor.o Exe
