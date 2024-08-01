objs = b_object.o b_stdlib.o gc.o b_lex.o b_ast.o compile.o parse.o

run: build
	./main.exe

build: $(objs)
	gcc -g $(objs) main.c -o main.exe

clean:
	-rm *.exe *.o *~
