OBJS = gc.o b_list.o b_string.o b_dict.o b_lexer.o ast.o b_parse.o b_func.o b_scope.o b_std.o
TGT = program16

run: build
	$(TGT)

build: $(OBJS)
	cc $(OBJS) v0-r1.c -o $(TGT)

clean:
	-del *.o *~ *.exe
