init: init.c read.c productor.c
	gcc -o init init.c 
	gcc -o read read.c 
	gcc -o productor productor.c -lpthread
