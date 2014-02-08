all:obj debug libsthread.a

obj:*.c testcases/barrier-test.c
	gcc -Wall -g  *.c testcases/barrier-test.c -o obj
debug:*.c testcases/barrier-test.c
	gcc -Wall -g  -D __STHREAD_DEBUG *.c testcases/barrier-test.c -o debug
libsthread.a:*.c
	gcc -c *.c
	ar rs libsthread.a *.o
clean:
	rm obj debug *.o libsthread.a
