init: init.c read.c productor.c killer.c
	gcc -o init init.c 
	gcc -o read read.c 
	gcc -o productor productor.c -lpthread
	gcc -o killer killer.c
