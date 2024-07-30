objs = b_object.o b_stdlib.c gc.c

run: build
	./main.exe

build: $(objs)
	gcc -g $(objs) main.c -o main.exe

clean:
	-del *.exe *.o *~
