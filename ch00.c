#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

int token;
char *src, *old_src;
int poolsize;
int line;

void next(){
    token = *src++;
}

void expression(int level){
    //do nothing
}

void program(){
    next();
    while(token > 0){
        printf("token is: %c\n", token);
        next();
    }
}

int eval(){
    return 0;
}

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

    src[i] = 0; //�������һ���ַ�Ϊ0��־����
    close(fd);


    program();
    return eval();
}
