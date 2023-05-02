.PHONY:all
all:myexe myload

myload:myload.c
	gcc -o $@ $^ -std=c99
myexe:myexe.c
	gcc -o $@ $^ -std=c99

.PHONY:clean
clean:
	rm -f myload myexe
