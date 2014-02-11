all:libsthread.a
libsthread.a:*.c
	mkdir objs
	gcc -Wall -g -c -D__STHREAD_DEBUG *.c
	ar rs libsthread.a *.o
	mv *.o objs
clean:
	rm -rf objs
	rm  libsthread.a
