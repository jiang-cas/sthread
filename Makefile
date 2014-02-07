all:obj debug

obj:*.c testcases/barrier-test.c
	gcc -Wall -g  *.c testcases/barrier-test.c -o obj
debug:*.c testcases/barrier-test.c
	gcc -Wall -g  -D __STHREAD_DEBUG *.c testcases/barrier-test.c -o debug
clean:
	rm obj debug
