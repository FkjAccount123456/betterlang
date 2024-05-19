OBJS = b_gc.o b_list.o b_string.o

run: compile
	program

compile: $(OBJS)
	clang $(OBJS) main.c -o program

clean:
	-del *.o *~
