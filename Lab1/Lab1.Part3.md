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

# Exercise 8

## 描述

We have omitted a small fragment of code - the code necessary to print octal numbers using patterns of the form "%o". Find and fill in this code fragment.

`cprintf()`中输出`%o`的部分代码被省去了，这段代码用于输出8进制数值。定位这段代码的位置，并尝试补全。

## 解答

想要理解这段代码先得理解好`kern/printf.c`、`lib/printfmt.c`和`kern/console.c`之间的关系。

我们调用的`cprintf()`和C标准库中的`printf()`很相似，它的任务是解析格式化字符串中`%`开始的内容，对应到随后的可变参数列表中去，并调用平台相关的控制台输出函数打印到控制台上去。`cprintf()`来源自`kern/printf.c`，它接受一个格式化字符串和一个可变参数列表。随后解析可变参数，把它和格式化字符串委派到`vprintf()`去。

`vprintf()`决定了使用哪个输出函数，并继续向下委派。对于不同的系统和平台来说，这个输出函数是操作系统自己实现的，但在现在这个情况下来说，我们得自己实现。因此，在这里就分叉出了两条路：输出的实现和格式化字符串解析的实现。

先看输出函数。现成的代码里写的是`putch()`，这个函数是对`cputchar()`的包装，加上了自己的计数实现。`cputchar()`来自`kern/console.c`，它是一个上层I/O函数，负责对下层I/O的包装。下层I/O函数是`cons_putc()`，它完成了对串行端口、打印终端和标准显示器的输出。

以标准显示器输出为例。它首先设置颜色属性，然后处理转义序列情况。字符内容不会立即输出，而是保存到一个缓冲区里，并使用一个指针确定当前位置。比如，`\b`将指针后移一位，前面一个字符就被抛弃了；`\n`将指针后移列数次，也就进入新行了；`\r`把指针移到行开始的位置；`\t`在缓冲区添加5个空白。处理完转义序列后，非转义序列字符就被放进了缓冲区。下一步要检查缓冲区有没有超过屏幕可容纳的大小，然后把溢出的行移到缓冲区开始的地方。最后，把缓冲区的内容使用outb指令传送到对应的I/O端口去。另外两个和显示器输出类似，都是对I/O端口的操作。

另一部分是解析格式化字符串的实现。这一部分委派到`vprintfmt()`来实现，这个函数来自`lib/printfmt.c`。它通过一个很大的switch解析`%`之后的内容，取得相应的可变参数内容，或者设置标志位。下一步是处理参数，如进制转化，然后根据标志位使用输出函数输出字符。

缺失的部分代码就来自`vprintfmt()`，位置是207行。下面是我的实现方法。

```c
        // (unsigned) octal
        case 'o':
            // Replace this with your code.
            putch('o', putdat);
            base = 8;
            num = getuint(&ap, lflag);
            goto number;
```

这部分代码和前面的`%d`和`%u`相似，区别在于`base`从10变成了8。

# Exercise 9

## 描述

Determine where the kernel initializes its stack, and exactly where in memory its stack is located. How does the kernel reserve space for its stack? And at which "end" of this reserved area is the stack pointer initialized to point to?

内核把自己的栈初始化到了哪里？它在内存中的位置具体是何处？内核是怎么预留自己的栈空间的？在这段预留空间尾部中，栈指针初始化时指向了哪里？

## 解答

程序栈和`ebp`、`esp`两个寄存器相关，这两个寄存器时在`entry.s`里面完成初始化的。具体为下面两行：

```s
movl    $0x0,%ebp            # nuke frame pointer
movl    $(bootstacktop),%esp
```

`ebp`初始化为`0x0`，而`esp`为`bootstacktop`的值。`bootstacktop`是在`entry.s`尾部数据部分里面`bootstack`部分定义的。由`bootstack`的`.space`可以得知栈的大小为KSTKSIZE = 8 * PGSIZE = 8 * 4096 = 0x8000。

`bootstacktop`的值在源代码中无法得知，但通过GDB可以查看到`esp`的值为`0xf0110000`。至此，可以确定内核的栈空间范围是F010 8000 ~ 0xF011 0000。

```
(gdb) si
=> 0xf0100039 <relocated+10>:   call   0xf0100094 <i386_init>
80              call    i386_init
(gdb) x/x $esp
0xf0110000 <entry_pgdir>:       0x00111021
```

至于内核为自己预留栈空间的方式，是`entry.s`中通过声明`bootstack`的`.space`来设置的32KiB空间。

栈指针指向的是栈顶，也就是`0xf0110000`。

esp和ebp两个寄存器是C语言编译的代码需要的寄存器。esp是x86栈指针(x86 Stack Pointer)，它总是指向栈的顶端。当新的数据入栈之后，esp的地址减少；当有数据出栈后，esp的地址增加。在32位机器上，增减量总是4的倍数。许多x86指令都要使用esp，如call。ebp是x86基指针(x86 Base Pointer)，它指向的是当前运行函数的地址。

C语言采用了函数式编程风格，因此和ebp、esp两个寄存器产生了关系。当调用一个函数时，先把所有的参数推入栈，然后把调用函数返回地址推入栈，转向调用函数地址，把当前函数基地址推入栈，再把栈顶指向基地址。C语言下的数据操作或多或少和栈相关，因此这里在进入i386_init()之前需要准备好栈环境。

# Exercise 10

# 描述

To become familiar with the C calling conventions on the x86, find the address of the test_backtrace function in obj/kern/kernel.asm, set a breakpoint there, and examine what happens each time it gets called after the kernel starts. How many 32-bit words does each recursive nesting level of test_backtrace push on the stack, and what are those words?

Note that, for this exercise to work properly, you should be using the patched version of QEMU available on the tools page or on Athena. Otherwise, you'll have to manually translate all breakpoint and memory addresses to linear addresses.

为了熟悉x86上C语言的调用机制，请在`obj/kern/kernel.asm`中找到`test_backtrace()`的地址，并在这里设置断点，观察内核调用它的时候发生了什么。每一级的`test_backtrace()`往栈上推了多少个32位字？这些字内容是什么？

## 解答

通过阅读`obj/kern/kernel.asm`可以发现，`test_backtrace()`位于0xf0100040。

每次调用都会往栈上推入20个32位字。这20字节保存的是程序实参值、局部变量等等，比如这里查看ebx的值为0x5，也就是`i386_init()`里面传来的参数。

根据GDB的输出信息来看，在第一次调用`test_backtrace()`的时候，ebp为空，esp为0xf0100100，这个地址是`i386_init()`调用`test_backtrace()`的位置，因此也就是函数的返回地址。至于ebp和esp指向的位置，根据上一个练习知道了内核的栈顶为0xF011 0000，向下扩展，0xf010fff8就是栈指针当前位置，0xf010ffcc就是当前函数的栈顶。

至于20字节空间大小的来源，来自地址0xf0100044的指令`sub  $0xc,%esp`，因此esp向下12字节空间。（不对啊，这里是0xc=12，哪儿来的20字节？？？）

`test_backtrace(4)`中，ebp的值设为了上次调用的ebp，而其位置则是继续向下开辟栈空间。`mov %esp, %ebp`指令把esp的值拷贝给ebp，也就相等了。


```
(gdb) x/x $ebp
0xf010fff8:     0x00000000
(gdb) x/x $esp
0xf010ffcc:     0xf0100100

(gdb) x/x $ebp
0xf010ffc8:     0xf010fff8
(gdb) x/x $esp
0xf010ffb8:     0x00000000

(gdb) x/x $ebp
0xf010ffa8:     0xf010ffc8
(gdb) x/x $esp
0xf010ff98:     0x00000000

(gdb) x/x $ebp
0xf010ff88:     0xf010ffa8
(gdb) x/x $esp
0xf010ff78:     0xf010ffa8

(gdb) x/x $ebp
0xf010ff68:     0xf010ff88
(gdb) x/x $esp
0xf010ff58:     0xf010ff88

(gdb) x/x $ebp
0xf010ff48:     0xf010ff68
(gdb) x/x $esp
0xf010ff38:     0xf010ff68

(gdb) x/x $ebp
0xf010ff28:     0xf010ff48
(gdb) x/x $esp
0xf010ff18:     0xf010ff48
```

# Exercise 11

## 描述

Implement the backtrace function as specified above. Use the same format as in the example, since otherwise the grading script will be confused. When you think you have it working right, run make grade to see if its output conforms to what our grading script expects, and fix it if it doesn't. After you have handed in your Lab 1 code, you are welcome to change the output format of the backtrace function any way you like.

If you use read_ebp(), note that GCC may generate "optimized" code that calls read_ebp() before mon_backtrace()'s function prologue, which results in an incomplete stack trace (the stack frame of the most recent function call is missing). While we have tried to disable optimizations that cause this reordering, you may want to examine the assembly of mon_backtrace() and make sure the call to read_ebp() is happening after the function prologue.

完成`backtrace()`函数，实现上述的功能。格式要和上述输出一致（这是课程提交的格式需求，和我没关系）。

如果你使用了read_ebp()，GCC可能对代码进行优化，也就是在mon_backtrace()的栈初始化（也就是函数前言，function prologue）之前就调用read_ebp()，这可能导致stack trace输出不完整（最近几次函数调用的栈帧丢失）。如果你关掉了代码优化，你可以检查一下汇编代码，确认调用顺序。

> 注：上述功能指的就是打印backtrace，输出内容包括ebp、eip和args。

## 解答

x86头文件里提供了read_ebp()函数，通过内联汇编代码的方式获取到ebp栈基址寄存器的内容。

在实验指导中有提到，eip即x86下的指令指针，它通常指向call指令后下一条指令的地址（The return instruction pointer typically points to the instruction after the call instruction），参考下面的例子：

```
(gdb) x/x $ebp
0xf010ffc8:     0xf010fff8
(gdb) x/x $ebp+4
0xf010ffcc:     0xf0100100
```

有一点值得注意的是，实验指导里提到的eip和GDB中获取的eip含义上存在出入。前者的含义刚刚提到过，而GDB中的eip指向的是下一条要执行的命令的地址。因此，为了获取实验中的eip，实际上要获取ebp+4。此外，实验指导还提醒过，(int)p + 1和(int)(p + 1)是完全不同的，在这里，栈内每个元素占4个字节，所以要加4。

这段输出内容取自第一次调用`test_backtrace()`并结束函数前言后得到的输出。在输出内容中可以看到，eip指向0xf0100100。通过阅读反编译代码可以看到，0xf0100100地址的上一条指令为0xf01000fb，内容正是调用`test_backtrace()`的指令。至于这么做的原因，我认为是为了被调用函数执行完成之后能够找到返回之后的执行地址。

从ebp开始，栈内先是返回地址，然后就是传递的参数了。基于此，就可以编写函数了。实现完成的代码如下

```c
int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	// Your code here.
	cprintf("Stack backtrace:\n");
	uint32_t* ebp = (uint32_t*)read_ebp();
	while (ebp)
	{
        // ebp[1]等价于*(ebp+1)，或者是*ebp+4
		cprintf("ebp=%08x, ", ebp[0]);
		cprintf("eip=%08x, ", ebp[1]);
		cprintf("args=%08x %08x %08x %08x %08x\n",
				ebp[2], ebp[3], ebp[4], ebp[5], ebp[6]);
		ebp = (uint32_t*) *ebp;
	}
	
	return 0;
}
```
