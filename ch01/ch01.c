
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
    return 0;
}

