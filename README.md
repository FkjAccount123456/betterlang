# betterlang
虽然只是一个玩具语言，但对我来说已经够好了

# 编译运行这个项目
```bash
make build
```

# 功能与示例

## 功能
- 变量
- 条件语句
- 循环语句
- 函数
- 内置函数
- 字符串
- 列表
- 词法作用域

## Hello, world!
```
Print("Hello, world!\n"); // Hello, world!
```

## 函数定义与调用
```
func sayHello(name) {
    Print("Hello, ", name, "!\n");
}
sayHello("world"); // Hello, world!
sayHello("betterlang"); // Hello, betterlang!
```

## 递归
```
func fac(num) {
    if num == 0 {
        return 1;
    } else {
        return num * fac(num - 1);
    }
}
Print(fac(5)); // 120
```

## 列表
```
list = [];
while (Len(list) < 10) {
    Append(list, Len(list) + 1);
    Print(list, "\n");
}
```

## 闭包
```
func Counter(start) {
    func inc() {
        start = start + 1;
        return start;
    }
    return inc;
}

var counter = Counter(0);
Print(counter()); // 1
Print(counter()); // 2
Print(counter()); // 3
```
