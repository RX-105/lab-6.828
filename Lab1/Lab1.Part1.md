# Exercise 2

## 描述

Use GDB's si (Step Instruction) command to trace into the ROM BIOS for a few more instructions, and try to guess what it might be doing. You might want to look at Phil Storrs I/O Ports Description, as well as other materials on the 6.828 reference materials page. No need to figure out all the details - just the general idea of what the BIOS is doing first.

使用GDB的步进指令`si`多追踪几次ROM BIOS运行的指令，试着猜猜它在干什么。不用非得搞明白细节，只要知道BIOS最开始大概在干什么就行了。

## 输出

```
[ CS : IP ]    ADDRESS: INSTRU ARGS
[f000:fff0]    0xffff0: ljmp   $0xf000,$0xe05b
[f000:e05b]    0xfe05b: cmpl   $0x0,%cs:0x6ac8
# 寄存器          地址      内容
# cs             0xf000   61440
[f000:e062]    0xfe062: jne    0xfd2e1
[f000:e066]    0xfe066: xor    %dx,%dx
[f000:e068]    0xfe068: mov    %dx,%ss
# dx             0x0      0
# ss             0x0      0
[f000:e06a]    0xfe06a: mov    $0x7000,%esp
# esp            0x0      0x0
[f000:e070]    0xfe070: mov    $0xf34c2,%edx
[f000:e076]    0xfe076: jmp    0xfd15c
```

注：物理地址计算方式为 physical address = 16 * segment + offset，使用16位地址。乘16等价于左移一位。

根据这些反编译代码，猜测如下：
1. 跳转到0xfe05b，当前所在地址接近1MiB，空间有限
2. 将0x0内容和0xf6ac8内容比较
3. 比较?和?，若不相等，跳转0xfd2e1
4. dx和dx做异或运算
5. 将ss的值复制给dx
6. 将esp的值写到0x7000
7. 将edx的值写到0xf34c2
8. 跳转到0xfd15c
