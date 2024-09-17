## CS 416 + CS 518 Project 1: Understanding the Stack and Basics (Warm-up Project)
### Due: 09/24/2024, Time: 11:59pm EST
### Points: 100 (5% of the overall course points.)

This simple warm-up project will help you recall basic systems programming
concepts before tackling the second project. The first part of this project
will help you recollect how stacks work. In part 2, you will use the Linux
pThread library to write a simple multi-threaded program. Part 3 involves
writing functions for simple bit manipulations. We have provided three code
files: *stack.c*, *threads.c*, and *bitops.c*, for completing the project.

*We will perform plagiarism checks throughout the semester. So, please do not violate  
Rutgers submission policy.*

### Part 1: Signal Handler and Stacks (35 points)
In this part, you will learn about signal handler and stack manipulation and also write a description 
of how you changed the stack.

#### 1.1 Description

In the skeleton code (*stack.c*), in the *main()* function, look at the line that dereferences memory address 0.
This statement will cause a segmentation fault.

```
r2 = *( (int *) 0 );
```
The first goal is to handle the segmentation fault by installing a signal
handler in the main function (*marked as Part 1 - Step 1 in stack.c*). If you register 
the following function correctly with the segmentation handler, Linux
will first run your signal handler to give you a chance to address the segment
fault.
```
**void signal_handle(int signalno)**
```

**Goal:** In the signal handler, you must make sure the segmentation fault does
not occur a second time. To achieve this goal, you must change the stack frame
of the main function; else, Linux will attempt to rerun the offending
instruction after returning from the signal handler. Specifically, you must
change the program counter of the caller such that the statement *printf("I
live again!")* after the offending instruction gets executed. No other
shortcuts are acceptable for this assignment.

**More details:** When your code hits the segment fault, it asks the OS what to
do. The OS notices you have a signal handler declared, so it hands the reins
over to your signal handler. In the signal handler, you get a signal number -
signalno as input to tell you which type of signal occurred. That integer is
sitting in the stored stack of your code that had been running. If you grab the
address of that int, you can then build a pointer to your code's stored stack
frame, pointing at the place where the flags and signals are stored. You can
now manipulate ANY value in your code's stored stack frame. Here are the
suggested steps:

Step 2. Dereferencing memory address 0 will cause a segmentation fault. Thus,
you also need to figure out the length of this bad instruction.

Step 3. According to x86 calling convention, the program counter is pushed on
stack frame before the subroutine is invoked. So, you need to figure out where
is the program counter located on stack frame. (Hint, use GDB to show stack)

Step 4. Construct a pointer inside fault hander based on signalno,
pointing it to the program counter by incrementing the offset you figured out
in Step 3. Then add the program counter by the length of the offending instruction 
you figured out in Step 1.

#### 1.2 Compiling for  32-bit (-m32)
To reduce the project's complexity, please compile the *stack.c* for
32-bit bypassing pass an additional flag to your gcc (*-m32*). 
The 32-bit compilation makes it a bit easier because, in an
x86 32-bit mode, the function arguments are always pushed to the stack before
the local variables, making it easier to locate *main()'s* program counter.

#### 1.3 Desired Output
    handling segmentation fault!
    result after handling segfault ...!

#### 1.4 Report
Please submit a report that answers the following question. Without the report, you will not receive points for this part.
1. What are the contents in the stack? Feel free to describe your understanding.
2. Where is the program counter, and how did you use GDB to locate the PC?
3. What were the changes to get the desired result?


#### 1.5 Tips and Resources

- Man Page of Signal: http://www.man7.org/linux/man-pages/man2/signal.2.html
- Basic GDB tutorial: http://www.cs.cmu.edu/~gilpin/tutorial/ 


### Part 2: Bit Manipulation (35 points)

#### 2.1 Description
Understanding how to manipulate bits is a crucial part of systems/OS
programming and will be required for subsequent projects. As a first step
toward mastering bit manipulation, you will write simple functions to extract
and set bits. We have provided a template file **bitops.c**.

Setting a bit means updating a bit to 1 if it is currently 0, while clearing a
bit means changing a bit from 1 to 0.

##### 2.1.1 Extracting Top-Order Bits
Your first task is to complete the *get_top_bits()* function to find the
top-order bits. For example, if the global variable *myaddress* is set to
`4026544704`, and you need to extract the top (outer) 4 bits (`1111`, which is
decimal 15), your function *get_top_bits()*, which takes *myaddress* and the
number of bits (*GET_BIT_INDEX*) to extract as input, should return 15.

##### 2.1.2 Setting and Getting Bits at a Specific Index
Setting and getting bits at a specific index is widely used across all OSes.
You will use these operations frequently in Projects 2, 3, and 4. You will
complete two functions: *set_bit_at_index()* and *get_bit_at_index()*. Remember
that each byte consists of 8 bits.

Before calling *set_bit_at_index()*, a bitmap array (i.e., an array of bits)
will be allocated as a character array, specified using the *BITMAP_SIZE*
macro. For example, if *BITMAP_SIZE* is set to 4, you can store 32 bits (8 bits
per character element). The *set_bit_at_index()* function passes the bitmap
array and the index (*SET_BIT_INDEX*) at which a bit must be set. The
*get_bit_at_index()* function checks if a bit is set at a specific index.
Note that setting the 0th bit could make a char either 128 or 1 (either 
you set 1000_0000 or 0000_0001); for the purposes of this project it does 
not matter which one you pick as long as it's consistent. However, going with
little-endian is the recommended method as it reflects how data is usually
stored.

For example, if you allocate a bitmap array of 4 characters, `bitmap[0]` would
refer to byte 0, `bitmap[1]` to byte 1, and so on.

Note that you will not modify the main function, only the three functions:
*get_top_bits()*, *set_bit_at_index()*, and *get_bit_at_index()*.

During project evaluation, we may change the values of *myaddress* or other
macros. You don’t need to handle too many corner cases for this project (e.g.,
setting *myaddress* or other macros to 0).

#### 2.2 Report
In your report, describe how you implemented the bit operations.


### Part 3: pThread Basics (30 points)
In this part, you will learn how to use Linux pthreads and update a
shared variable. We have given a skeleton code (*thread.c*).

To use the Linux pthread library, you must compile your C files by linking
to the pthread library using *-lpthread* as a compilation option.

#### 3.1 Description
In the skeleton code, there is a global variable `x`, which is initialized to 0.
You are required to use pThread to create 2 threads to increment the global
variable by 1.

Use *pthread_create* to create two worker threads, and each of them will execute
*add_counter*. Inside *add_counter*, each thread increments `x`
10,000 times.

After the two worker threads finish incrementing counters, you must print the
final value of `x` to the console. Remember, the main thread may terminate before
the worker threads; avoid this by using *pthread_join* to let the main thread
wait for the worker threads to finish before exiting.

*Because `x` is a shared variable, you need a pthread mutex to guarantee that
each thread exclusively updates that shared variable.*
You can read about using pthread mutex in
the link [3]. In later lectures and projects, we will discuss how to use other
complex synchronization methods.

If you have implemented the thread synchronization correctly, the final value of
`x` would be 2 times the loop value.

*NOTE: When evaluating your projects, we will test your code for different loop
values.*

#### 3.2 Tips and Resources
- [1] pthread_create: http://man7.org/linux/man-pages/man3/pthread_create.3.html
- [2] pthread_join: http://man7.org/linux/man-pages/man3/pthread_join.3.html
- [3] pthread_mutex: https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html


#### 3.3 Report
You do not have to describe this part in the project report.


## FAQs

### Q1: Project 1 Submission

#### A:

For submission, create a zip file named `project1.zip` and upload it to Canvas.
The zip file should contain the following files. Please ensure that the report
file is a PDF (do not submit Word or text files).

`project1.zip` (in lowercase)
├── stack.c  
├── thread.c  
├── bitops.c  
└── report.pdf  

- Only one group member should submit the code. However, in your project report
  and at the top of your code files, list the names and NetIDs of all group
members, the course number (CS 416 or CS 518), and the iLab machine on which
you tested your code, as a comment at the top of the file.
  
- Your code must work on one of the iLab machines. Use the provided C code as a
  base and its functions. You may modify the function signature for Part 2 if
necessary.

### Q2: Using `signalno`

1. Should we use and modify the address of `signalno` (or a local pointer to
`signalno`)?  2. Can we use other addresses (e.g., the main function stack
pointer)?

#### A:

1. Yes, you should use `signalno`. While we prefer using `signalno` directly,
using a local pointer to `signalno` in the handler works the same.  2. No, you
cannot. You should use **`signalno` as your pointer to the stack** and
manipulate the stack accordingly.

### Q3: Using Other Packages  Can we use other packages such as `asm.h` to
manipulate the registers?

#### A:

No, we expect you to use only the packages included in the provided files.
**The goal of the project is to understand the stack, learn how registers are
stored on the stack, and use GDB to inspect stack frames** (as well as to get
comfortable using GDB in general). While `asm.h` can be used to manipulate
registers, it is not the solution we expect, and submissions using `asm.h` (or
other non-included packages) will have points deducted.

