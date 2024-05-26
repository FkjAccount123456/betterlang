OBJS = gc.o b_list.o b_string.o

run: compile
	program

build: $(OBJS)
	clang $(OBJS) v0-r1.c -o program

clean:
	-del *.o *~
