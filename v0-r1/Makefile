OBJS = gc.o b_list.o b_string.o b_dict.o b_lexer.o ast.o b_parse.o

run: compile
	program

build: $(OBJS)
	cc $(OBJS) v0-r1.c -o program

clean:
	-del *.o *~ *.exe
