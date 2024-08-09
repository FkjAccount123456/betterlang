# betterlang v2
[README.md](README.md) | README-en.md
## Brief Introduction
betterlang is a dynamic programming language written by C, compiled to bytecode.  
It has its own GC (Mark-Sweep), and the syntax looks like JavaScript.     
Unicode is not supported.  
## Compile & Run this Project  
``` bash
git clone https://github.com/FkjAccount123456/betterlang
cd betterlang
make build
./main hello.bl
```
## Changes Compared to betterlang v1
- Use linked list instead of list to store GC data
  - GC data of deleted objects can be removed
  - and they won't take up too much space
- Use hash map instead of Trie to implement Dict
  - Memory usage can be reduced, but it may run slower (proceed hash conflicts)
- Compile the AST instead of token sequence
  - Easier to be extended and to debug
- Add variable hoist
  - You must know it if you have learnt JavaScript
- More features...
  - Please wait and see...
## Type System
- int（long long in C）
- float（double in C）
- string
- list
- dict/object
- function
- builtin function
- method
## Some Instances
### Hello, world!
``` js
println("Hello, world!");
  ```
### Integers and floats operation
``` js
println(1 + 2);
println(0.1 + 0.2);
```
### If Statement
``` js
if 0      { println(0); }
else if 1 { println(1); }
else      { println(2); }
```
### while-continue-break
``` js
var i = 0;
while 1 {
    i = i + 1;
    if i == 5  { continue; }
    if i == 10 { break; }
    println(i);
}
```
### Functions and Recursive
``` js
func fac(num) {
    if num == 0 { return 1; }
    else        { return fac(num - 1) * num; }
}

var a = 10;
println(fac(a));
```
### Function Closure
``` js
func Counter(start) {
    func next() {
        start = start + 1;
        return start;
    }
    return next;
}

var counter = Counter(0);
var x = counter();
while x < 10 {
    println(x);
    x = counter();
}
```
### Lambda Expression
``` js
var fn1 = func(a) { return a + 1; };
println(fn1(1));
```
### Object and Method Call
``` js
var obj = {
    a: 10,
    b: 20,
    print: func(self) {
        println("a: ", self["a"], ", b: ", self["b"]);
        return none;
    },
    setA: func(self, a) {
        self["a"] = a;
        return none;
    },
};

obj.print();
obj.setA(30);
obj.print();
```
