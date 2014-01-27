all:obj debug

obj:*.c testcases/test2.c
	gcc -Wall -pthread *.c testcases/test2.c -o obj
debug:*.c testcases/test2.c
	gcc -Wall -g -pthread -D __STHREAD_DEBUG *.c testcases/test2.c -o debug
clean:
	rm obj debug
