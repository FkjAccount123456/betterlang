objs = b_object.c b_stdlib.c gc.c b_lex.c b_ast.c compile.c parse.c vm.c builtins.c

run: build
	./main.exe

build: $(objs)
	gcc -g $(objs) main.c -o main.exe

clean:
	-rm *.exe *.o *~
