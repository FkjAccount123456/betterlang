# 2024-6-26
垃圾回收体系就是一个图，大概可以用邻接表这样的东西存
每个垃圾回收对象只需要存它的指针和回收函数
但Dict List之类里面还会套垃圾回收对象、、、
那就只能在Dict List之类里面加入一个虚垃圾回收对象、、、
燃烧内存吧！！！
# 2024-6-27
（GC v3）注意传递对象的时候可以在外部新建一个一样的对象并建立双向引用
# 2024-6-28
if-else if-else的编译
```
if      cond1 { body1 }
else if cond2 { body2 }
else          { body3 }

cond1
jnz pos1
body1
jmp endpos
pos1:
cond2
jnz pos2
body2
jmp endpos
pos2:
body3
endpos:
```
while的编译
```
while cond {
    break;
    continue;
}
begin:
cond
jnz end
jmp end
jmp begin
jmp begin
end:
```
begin用一个stack存
end和if的endpos存储差不多，但是为了区分多个while必须在stack之中加上NULL
、、、，这些都得存Parser里，还得再写个SizeList

# 2024-6-30
不知道为啥别的都能free掉就引号里的字符串不行
## 对了，补充一点betterlang实现设计
- ObjTrait体系直接继承的betterlang v0
- 单趟式编译（不解析生成AST）到字节码（说实话if while这些真不好编译）（终于知道为什么Lua没有continue了）（
- 甚至因为编译的原因我直接用的函数作用域（避免计算break/continue的时候要退出多少层作用域）（当然也是模仿Python）
- 使用标记清除GC，虽然实现的很烂
- GC直接当成有向图，拿邻接表存（看来不能说OI没用，这不是数据结构和算法吗）（
- 隐式引用传递（动态语言基本都有的特性）一个对象用双向引用实现，前面说了

