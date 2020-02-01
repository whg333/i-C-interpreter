
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

int token;              //当前读取的token字符
char *src, *old_src;    //指向读取文件内容的指针
int poolsize;           //默认分配的内存大小byte
int line;               //行数

int *text, *old_text,   //代码段（无符号整型）
    *stack;             //函数调用栈
char *data;             //数据段

int *pc,                //程序计数器
    *sp,                //栈地址指针寄存器
    *bp,                //基址指针
    ax,                 //通用寄存器，存放一条指令执行后的结果
    cycle;

enum {
    LEA, IMM, JMP, CALL, JZ, JNZ, ENT, ADJ, LEV, LI, LC, SI, SC, PUSH,
    OR, XOR, AND, EQ, NE, LT, GT, LE, GE, SHL, SHR, ADD, SUB, MUL, DIV, MOD,
    OPEN, READ, CLOS, PRTF, MALC, MSET, MCMP, EXIT,
};

/**
+------------------+
|    stack   |     |      high address
|    ...     v     |
|                  |
|                  |
|                  |
|                  |
|    ...     ^     |
|    heap    |     |
+------------------+
| bss  segment     |
+------------------+
| data segment     |
+------------------+
| text segment     |      low address
+------------------+
*/

int main(int argc, char **argv){
    int i, fd;

    argc--;
    argv++;

    poolsize = 256 * 1024;
    line = 1;

    printf("open(%s)\n\n", *argv);
    if((fd = open(*argv, 0)) < 0){
        printf("could not open(%s)\n", *argv);
        return -1;
    }

    if(!(src = old_src = malloc(poolsize))){
        printf("could not malloc(%d) for source area\n", poolsize);
        return -1;
    }

    i = read(fd, src, poolsize-1);
    printf("read() return %d\n\n", i);
    if(i <= 0){
        return -1;
    }

    src[i] = 0; //设置最后一个字符为0代表结束，program函数执行的时候判断
    close(fd);

    if(!(text = old_text = malloc(poolsize))){
        printf("could not malloc(%d) for text area\n", poolsize);
        return -1;
    }
    if(!(data = malloc(poolsize))){
        printf("could not malloc(%d) for data area\n", poolsize);
        return -1;
    }
    if(!(stack = malloc(poolsize))){
        printf("could not malloc(%d) for stack area\n", poolsize);
        return -1;
    }

    memset(data, 0, poolsize);
    memset(text, 0, poolsize);
    memset(stack, 0, poolsize);

    bp = sp = (int *)((int)stack + poolsize);
    ax = 0;

    i = 0;
    text[i++] = IMM;
    text[i++] = 10;
    text[i++] = PUSH;
    text[i++] = IMM;
    text[i++] = 20;
    text[i++] = ADD;
    text[i++] = PUSH;
    text[i++] = EXIT;
    pc = text;

    program();
    return eval();
}

void program(){
    next();
    while(token > 0){
        printf("token is: %c\n", token);
        next();
    }
}

void next(){
    token = *src++;
}

void expression(int level){
    //do nothing
}

int eval(){
    int op, *tmp;
    while(1){
        op = *pc++;
        printf("op is: %d\n", op);

        //通用寄存器和跳转
        if(op == IMM){
            //load immediate value to ax
            ax = *pc++;
        }else if(op == LC){
            //load character to ax, address in ax
            ax = *(char *)ax;
        }else if(op == LI){
            //load integer to ax, address in ax
            ax = *(int *)ax;
        }else if(op == SC){
            //save character to address, value in ax, address on stack
            *(char *)*sp++ = ax;
        }else if(op == SI){
            //save integer to address, value in ax, address on stack
            *(int*)*sp++ = ax;
        }else if(op == PUSH){
            //push the value of ax onto the stack
            *--sp = ax;
        }else if(op == JMP){
            //jump to the address
            pc = (int *)*pc;
        }else if(op == JZ){
            //jump if ax is zero
            pc = ax ? pc + 1 : (int *)*pc;
        }else if(op == JNZ){
            //jump if ax is not zero
            pc = ax ? (int *)*pc : pc + 1;
        }

        //子函数调用
        else if(op == CALL){
            //call subroutine
            *--sp = (int)(pc + 1);
            pc = (int *)*pc;
        }else if(op == ENT){
            //make new stack frame
            *--sp = (int)bp;
            bp = sp;
            sp = sp - *pc++;
        }else if(op == ADJ){
            //remove arguments from frame --> add esp, <size>
            sp = sp + *pc++;
        }else if(op == LEV){
            //restore call frame and PC
            sp = bp;
            bp = (int *)*sp++;
            pc = (int *)*sp++;
        }else if(op == LEA){
            //load address for arguments
            ax = (int)(bp + *pc++);
        }

        //运算符指令
        else if(op == OR){
            ax = *sp++ | ax;
        }else if(op == XOR){
            ax = *sp++ ^ ax;
        }else if(op == AND){
            ax = *sp++ & ax;
        }else if(op == EQ){
            ax = *sp++ == ax;
        }else if(op == NE){
            ax = *sp++ != ax;
        }else if(op == LT){
            ax = *sp++ < ax;
        }else if(op == LE){
            ax = *sp++ <= ax;
        }else if(op == GT){
            ax = *sp++ > ax;
        }else if(op == GE){
            ax = *sp++ >= ax;
        }else if(op == SHL){
            ax = *sp++ << ax;
        }else if(op == SHR){
            ax = *sp++ >> ax;
        }else if(op == ADD){
            ax = *sp++ + ax;
        }else if(op == SUB){
            ax = *sp++ - ax;
        }else if(op == MUL){
            ax = *sp++ * ax;
        }else if(op == DIV){
            ax = *sp++ / ax;
        }else if(op == MOD){
            ax = *sp++ % ax;
        }

        //内置函数
        else if(op == EXIT){
            printf("exit(%d)\n", *sp);
            return *sp;
        }else if(op == OPEN){
            ax = open((char *)sp[1], sp[0]);
        }else if(op == CLOS){
            ax = close(*sp);
        }else if(op == READ){
            ax = read(sp[2], (char *)sp[1], *sp);
        }else if(op == PRTF){
            tmp = sp + pc[1];
            ax = printf((char *)tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5], tmp[-6]);
        }else if(op == MALC){
            ax = (int)malloc(*sp);
        }else if(op == MSET){
            ax = (int)memset((char *)sp[2], sp[1], *sp);
        }else if(op == MCMP){
            ax = memcmp((char *)sp[2], (char *)sp[1], *sp);
        }else{
            printf("unknown instruction:%d\n", op);
            return -1;
        }
    }
    return 0;
}

