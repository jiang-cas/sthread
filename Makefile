all:obj-test
obj-test:*.c test/test2.c
	gcc -Wall *.c test/test2.c -o obj-test
clean:
	rm obj-test
