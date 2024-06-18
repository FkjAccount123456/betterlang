objs = b_gc.o b_obj.o

run: build
	./main.exe

build: $(objs)
	gcc $(objs) main.c -o main.exe

%.o: %.c
	gcc -c $< -o $@

clean:
	-del *.exe *.o *~
