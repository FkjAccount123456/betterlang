objs = b_gc.c b_obj.c b_lex.c

run: build
	./main.exe

build: $(objs)
	gcc -g $(objs) main.c -o main.exe

clean:
	-del *.exe *.o *~
