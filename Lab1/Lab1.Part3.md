# Exercise 7

## 描述

Use QEMU and GDB to trace into the JOS kernel and stop at the movl %eax, %cr0. Examine memory at 0x00100000 and at 0xf0100000. Now, single step over that instruction using the stepi GDB command. Again, examine memory at 0x00100000 and at 0xf0100000. Make sure you understand what just happened.

What is the first instruction after the new mapping is established that would fail to work properly if the mapping weren't in place? Comment out the movl %eax, %cr0 in kern/entry.S, trace into it, and see if you were right.

使用Qemu和GDB去追踪JOS内核文件，并且停止在movl %eax, %cr0指令前。此时看一下内存地址0x00100000以及0xf0100000处分别存放着什么。然后使用stepi命令执行完这条命令，再次检查这两个地址处的内容。确保你真的理解了发生了什么。

如果这条指令movl %eax, %cr0并没有执行，而是被跳过，那么第一个会出现问题的指令是什么？我们可以在entry.S里把这条命令注释掉来验证一下。

## 解答

输出如下：

```
=> 0x100025:    mov    %eax,%cr0
(gdb) x/x 0x00100000
0x100000:       0x1badb002
(gdb) x/x 0xf0100000
0xf0100000 <_start+4026531828>: 0x00000000

=> 0x100028:    mov    $0xf010002f,%eax
(gdb) x/x 0x00100000
0x100000:       0x1badb002
(gdb) x/x 0xf0100000
0xf0100000 <_start+4026531828>: 0x1badb002
```
可见`0x00100000`和`0xf0100000`的内容最开始是不同的，`0x100025`处的指令执行完之后就变得一样了。可以知道，低端内存和高端内存产生了映射。

下面把`mov %eax,%cr0`注释掉试试：

```
=> 0x100025:    mov    $0xf010002c,%eax
(gdb) x/x 0x00100000
0x100000:       0x1badb002
(gdb) x/x 0xf0100000
0xf0100000 <_start+4026531828>: 0x00000000
```

`0xf0100000`处的值仍然为0，没有发生改变，也就是不存在映射。当后面需要访问高端内存的时候，就会触发程序异常退出。


