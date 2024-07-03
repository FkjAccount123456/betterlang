# betterlang-v1
一个玩具语言
编译到字节码，具有标记清除GC（v0是引用计数GC）
动态语言
## 使用此项目
编译（须有gcc和make）
``` bash
make build
```
运行
``` bash
./main test.bl
```
## 类型系统
- 整数（C语言long long）
- 浮点数（C语言double）
- 字符串
- 列表
- 字典（用字典树实现，内存占用相当大）
- 函数
- 方法（不可手动构造，在引用字典中的函数时自动构造）
- 内置函数
## 内置函数
- print/println 输出函数，println比前者多输出一个换行
- len 获取列表、字符串的长度
- append 给列表、字符串加入一个元素、字符
- getchar 从stdin读取一个字符
## 语法及示例
### if-else if-else
```
if 0      { println(0); }
else if 1 { println(1); }
else      { println(2); }
```
### while break continue
```
var i = 0;
while 1 {
    i = i + 1;
    if i == 5  { continue; }
    if i == 10 { break; }
    println(i);
}
```
### 列表
```
var l = [1, 2, [2, 4, 5]];
println(l, " ", l[2], " ", l[2][0]);
l[2][0] = 3;
println(l, " ", l[2], " ", l[2][0]);
println("len(l): ", len(l));

var l1 = [], j = 0;
while j < 10 {
    j = j + 1;
    append(l1, j);
    println(l1, " ", j);
}
```
### 函数
```
func hello(name) {
    println("Hello, ", name, "!");
    return none; // 注意，一个函数必须确保最后返回一个值，否则会出错
}
hello("world"); // 函数调用参数如果有误编译时不会报错，但运行时一定会出错
hello("betterlang");
```
### 递归
```
func fac(num) {
    if num == 0 { return 1; }
    else        { return fac(num - 1) *num; }
}
```
### 匿名函数
```
var fn1 = func(a) { return a + 1; };
println(fn1(1));
```
### 闭包
```
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
### 字典
```
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
