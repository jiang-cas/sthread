all:libsthread.a
libsthread.a:*.c
	gcc -c *.c
	ar rs libsthread.a *.o
clean:
	rm *.o libsthread.a
