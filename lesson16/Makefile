obj=mytest.o add.o sub.o

mytest:$(obj)    #展开.o文件
	gcc -o $@ $^
%.o:%.c
	gcc -c $<
%.o:./test_lib/%.c
	gcc -c $<

.PHONY:clean
clean:
	rm -f *.o mytest
