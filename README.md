# betterlang v2
[README.md](README.md) | README-en.md
## 简介
betterlang是一个用C编写的、编译到字节码的动态语言  
具有自己的GC（标记-清除）  
语法类似早期JavaScript  
不支持宽字符  
## 编译运行此项目
``` bash
git clone https://github.com/FkjAccount123456/betterlang
cd betterlang
make build
./main hello.bl
```
## 相比v1的更新内容
- GC图存储方式由连续表结构变为链表
  - 内存回收后GC中的内存记录会被删除
  - 避免出现无内存泄漏却内存占用大的情况
- 字典实现方式由字典树变为哈希表
  - 减少内存消耗，但速度会减慢（处理哈希冲突）
- 从基于tokens直接编译变为先语法分析再进行编译
  - 便于扩展和调试
- 加入变量提升
  - 熟悉JavaScript的一定知道
- 更多的功能
  - 敬请期待betterlang后续的小版本
## 类型系统
- int（C语言long long）
- float（C语言double）
- string
- list
- dict/object
- function
- builtin function
- method
## 语法示例
### Hello, world!
``` js
println("Hello, world!");
  ```
### 整数和浮点数运算
``` js
println(1 + 2);
println(0.1 + 0.2);
```
### if条件分支
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
### 函数与递归
``` js
func fac(num) {
    if num == 0 { return 1; }
    else        { return fac(num - 1) * num; }
}

var a = 10;
println(fac(a));
```
### 函数闭包
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
### 匿名函数
``` js
var fn1 = func(a) { return a + 1; };
println(fn1(1));
```
### 对象（字典）和方法调用
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
