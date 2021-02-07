CC=gcc -g -I.

%.o:%.c
	$(CC) -c $^ -o $@

sync:sync.o
	$(CC) $^ -o $@