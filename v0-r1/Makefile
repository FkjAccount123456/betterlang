OBJS = gc.o b_list.o b_string.o b_dict.o b_lexer.o ast.o b_parse.o b_func.o

run: compile
	program

build: $(OBJS)
	cc $(OBJS) v0-r1.c -o program

clean:
	-del *.o *~ *.exe
