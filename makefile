#Resource: http://docstore.mik.ua/orelly/linux/run/ch13_02.htm
#Woody Chen Lin Chang, 301 231 083, Assignment 1.5 Makefile
#execution: ./proc_parse
proc_parse: main.o
	gcc -o my_shell main.o

main.o: main.c
	gcc -c main.c
