# Exercise 3

## 描述

Take a look at the lab tools guide, especially the section on GDB commands. Even if you're familiar with GDB, this includes some esoteric GDB commands that are useful for OS work.

Set a breakpoint at address 0x7c00, which is where the boot sector will be loaded. Continue execution until that breakpoint. Trace through the code in boot/boot.S, using the source code and the disassembly file obj/boot/boot.asm to keep track of where you are. Also use the x/i command in GDB to disassemble sequences of instructions in the boot loader, and compare the original boot loader source code with both the disassembly in obj/boot/boot.asm and GDB.

Trace into bootmain() in boot/main.c, and then into readsect(). Identify the exact assembly instructions that correspond to each of the statements in readsect(). Trace through the rest of readsect() and back out into bootmain(), and identify the begin and end of the for loop that reads the remaining sectors of the kernel from the disk. Find out what code will run when the loop is finished, set a breakpoint there, and continue to that breakpoint. Then step through the remainder of the boot loader.

仔细阅读实验工具指南。

在地址`0x7c00`处设置断点，在这里将会加载启动扇区的内容。在触发这个断点之前继续运行。

继续追踪`boot/boot.S`的代码，使用源代码和`obj/boot/boot.asm`里面的反编译代码确认你目前的位置。

使用`x/i`命令反编译引导加载器中的指令，将其和`obj/boot/boot.asm`中的代码做一个对比。

## 输出

```
[   0:7c00] => 0x7c00:  cli

Breakpoint 1, 0x00007c00 in ?? ()
```
这个位置处于引导加载器的开始，断点处为`boot.S`的第一条指令，第15行。

## 问题

1. 什么时候开始执行32位代码？具体是什么原因导致从16位切换到32位的？

在地址0x7c32处开始执行32位代码。从seta20.1段开始打开了A20地址线，然后地址0x7c2d时ljmp指令跳入到地址0x7c32执行，此处为32位代码段，因此就进入了32位保护模式。

2. 引导加载器执行的最后一条指令是什么？内核加载时执行的第一条指令是什么？

```
=> 0x7d6b:      call   *0x10018
```
上面是引导加载器执行的最后一条指令，对应`boot/main.c`的第60行代码。从此开始，当前地址将跳到1MiB以上，进入内核的执行。

```
=> 0x10000c:    movw   $0x1234,0x472
```
上面是内核执行的第一条指令。

3. 内核执行的第一条指令地址在哪里？

`0x10000c`

4. 引导加载器是怎么知道要加载多少个扇区用于从磁盘加载内核的？你是从哪里看出来的？

根据`boot/main.c`的`bootmain()`可以看出，根据`ELFHDR`的程序头数量进行for循环，循环多少次就加载多少次扇区。

# Exercise 4

## 描述

阅读并运行下列代码。理解输出的数值是怎么来的。特别要理解第1和第6行的地址是怎么来的，第2到第4行的数值是怎么得到的，为什么第5行的输出结果出了问题。

```c
#include <stdio.h>
#include <stdlib.h>

void f(void)
{
    int a[4];
    int *b = malloc(16);
    int *c;
    int i;

    printf("1: a = %p, b = %p, c = %p\n", a, b, c);

    c = a;
    for (i = 0; i < 4; i++)
        a[i] = 100 + i;
    c[0] = 200;
    printf("2: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
           a[0], a[1], a[2], a[3]);

    c[1] = 300;
    *(c + 2) = 301;
    3 [c] = 302;
    printf("3: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
           a[0], a[1], a[2], a[3]);

    c = c + 1;
    *c = 400;
    printf("4: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
           a[0], a[1], a[2], a[3]);

    c = (int *)((char *)c + 1);
    *c = 500;
    printf("5: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
           a[0], a[1], a[2], a[3]);

    b = (int *)a + 1;
    c = (int *)((char *)a + 1);
    printf("6: a = %p, b = %p, c = %p\n", a, b, c);
}

int main(int ac, char **av)
{
    f();
    return 0;
}

```

## 解答

输出结果为：

```
1: a = 000000000065FDC0, b = 0000000000026C10, c = 0000000000000010
2: a[0] = 200, a[1] = 101, a[2] = 102, a[3] = 103
3: a[0] = 200, a[1] = 300, a[2] = 301, a[3] = 302
4: a[0] = 200, a[1] = 400, a[2] = 301, a[3] = 302
5: a[0] = 200, a[1] = 128144, a[2] = 256, a[3] = 302
6: a = 000000000065FDC0, b = 000000000065FDC4, c = 000000000065FDC1
```
第1行的输出地址均为系统分配的虚拟地址；第6行中b的地址由a加1后强制类型转换为指针整型得到的地址，32位机的整型占4个字节，因此b的地址等于a的地址加4；c的地址由强制类型转换为指针字符型的a加1后再强制类型转换为指针整型后得到的地址，字符型占1字节，因此c的地址等于a的地址加1。

第2行，c的地址等于a[0]的地址，因此第2行a[0]等于200。

第3行中均为通过指针的方式修改数组a的数值。

第4行中，`c = c + 1`中的1隐含为整型，这行代码的含义为c的地址加4，也就是指向a[1]，然后改变了它的值。

第5行中，c的地址加了1，如果这个时候通过c的地址写值，实际上写的是a[1]的后3个字节加a[2]的第一个字节，这样不标准的写法就会导致数据混乱。

# Exercise 5

## 问题

Trace through the first few instructions of the boot loader again and identify the first instruction that would "break" or otherwise do the wrong thing if you were to get the boot loader's link address wrong. Then change the link address in boot/Makefrag to something wrong, run make clean, recompile the lab with make, and trace into the boot loader again to see what happens. Don't forget to change the link address back and make clean again afterward!

再一次追踪一下引导加载器的一开始的几句指令，找到第一条满足如下条件的指令处：

当我修改了引导加载器的链接地址，这个指令就会出现错误。

找到这样的指令后，把引导加载器的链接地址修改一下，我们要在`boot/Makefrag`文件中修改它的链接地址，修改完成后运行`make clean`， 然后通过`make`指令重新编译内核，再找到那条指令看看会发生什么。最后别忘了改回来。

## 解答

本次练习中，我把程序入口点地址从`0x7c00`改成了`0x7e00`。

打开模拟器运行。由于程序被BIOS放在`0x7c00`运行，所以前面的指令没有问题。但是涉及到绝对地址的指令就会出错，如下：

```
[   0:7c1e] => 0x7c1e:  lgdtw  0x7e64

(gdb) x/6xb 0x7e64
0x7e64: 0x00    0x00    0x00    0x00    0x00    0x00
```
这里的地址没有改变，但是程序起始地址加了200，这里的地址不一起改的话就会出错。然后我又用GDB查看`0x7e64`后6位内容就可以看到全都是0，想都不用想GDTR表肯定没加载成功。

接下来有问题的指令如下：
```
[   0:7c2d] => 0x7c2d:  ljmp   $0x8,$0x7e32
```
已知引导加载器的范围为`0x7c00`开始的512个字节，因此只能读写到`0x7dff`为止。但由于我修改程序让起始地址多了200，这里长跳指令的参数二地址也就相应的变成了`0x7e32`，明显超出了范围，导致无法跳转。引导加载器也就死在这儿了。（跳不进去，怎么想都跳不进去的吧！）

最终也就导致了触发CPU关闭，运行结束。

# Exercise 6

## 描述

We can examine memory using GDB's x command. The GDB manual has full details, but for now, it is enough to know that the command x/Nx ADDR prints N words of memory at ADDR. (Note that both 'x's in the command are lowercase.) Warning: The size of a word is not a universal standard. In GNU assembly, a word is two bytes (the 'w' in xorw, which stands for word, means 2 bytes).

Reset the machine (exit QEMU/GDB and start them again). Examine the 8 words of memory at 0x00100000 at the point the BIOS enters the boot loader, and then again at the point the boot loader enters the kernel. Why are they different? What is there at the second breakpoint? (You do not really need to use QEMU to answer this question. Just think.)

重新启动模拟器和GDB，然后用`x/Nx ADDR`命令查看`0x00100000`处的八个字节，然后继续运行到内核加载完成，再次查看那八个字节。为什么这两次存在区别？第二次断点的时候有些什么？（你甚至可以不用模拟器，直接想出来。）

## 解答

模拟器刚启动的时候，BIOS将0扇区处的引导加载器加载到内存，此时运行在实模式下，1MiB以上的内存空间无法访问，因此`0x00100000`处全都是0。

内核加载完毕之后，`0x00100000`处实际上是`0xf0100000`处的指令。因为内核程序的VMA在`0xf0100000`，LMA在`0x00100000`，VMA处的指令将会加载到LMA处。
