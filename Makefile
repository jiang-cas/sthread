all:obj debug

obj:*.c testcases/test2.c
	gcc -Wall -g  *.c testcases/test2.c -o obj
debug:*.c testcases/test2.c
	gcc -Wall -g  -D __STHREAD_DEBUG *.c testcases/test2.c -o debug
clean:
	rm obj debug
