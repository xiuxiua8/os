all : p call t
.PHONY : all

p: p.c 
	gcc -o p p.c 

call: call.c
	gcc -o call call.c

t: t.c
	gcc -o t t.c -lpthread
