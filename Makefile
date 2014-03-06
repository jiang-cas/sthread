all:libsthread.a
libsthread.a:*.c
	mkdir objs
#	gcc -Wall -D__STHREAD_DEBUG -g -c *.c
	gcc -Wall -g -c *.c
	ar rs libsthread.a *.o
	mv *.o objs
clean:
	rm -rf objs
	rm  libsthread.a
