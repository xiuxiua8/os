all : p call t call2 spinlock
.PHONY : all

p: p.c 
	gcc -o p p.c 

call: call.c
	gcc -o call call.c

t: t.c
	gcc -o t t.c -lpthread

call2: call2.c
	gcc -o call2 call2.c

spinlock: spinlock.c
	gcc -o spinlock spinlock.c