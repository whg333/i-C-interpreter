
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

int token;              //当前读取的token字符
char *src, *old_src;    //指向读取文件内容的指针
int poolsize;           //默认分配的内存大小byte
int line;               //行数

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

