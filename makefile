#execution: ./proc_parse
proc_parse: main.o
	gcc -o my_shell main.o

main.o: main.c
	gcc -c main.c
