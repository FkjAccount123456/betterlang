objs = 

run: build
	./main.exe

build: $(objs)
	gcc -g $(objs) main.c -o main.exe

clean:
	-del *.exe *.o *~
