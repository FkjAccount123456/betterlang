objs = gc.c obj.c lex.c compile.c vm.c idict.c

run: build
	./main.exe test.bl

build: $(objs)
	gcc -g $(objs) main.c -o main.exe

clean:
	-del *.exe *.o *~
