#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include <stdbool.h>

int
main(int argc, char *argv[])
{
    if(argc>2){
        printf("args err: you should pass 0 argument. \n");
        exit(1);
    }
    
    int p1[2];
    pipe(p1); // parent -> child ; 在数组p中， 0：读端，1：写端。 在 | 中， 左边为写，右边为读。

    for(int i=2;i<=35;i++){
        write(p1[1], &i, 4);
    }
    int pid = fork(); //需要在close之前fork，这样才能拷贝变量、管道文件描述符。

    if(pid==0){
        serie(p1);
        exit(0);
    }else{
        close(p1[1]);
        close(p1[0]); 
        wait((int *) 0); //传递0地址，表示父进程不关心子进程的退出状态。
        exit(0);
    }
}
/*
    持续接收左端的值，将第一个值val1打印，剩余的如果无法整除val1的均传给右端。
    然后开启子进程。
*/
void serie(int p1[]){
    close(p1[1]); //先关掉写端。确保能读到最后。
    int *re = malloc(sizeof(int));
    read(p1[0], re, 4); // 读取第一个数
    int n = *re;
    printf("prime %d\n",n);

    int p2[2];
    pipe(p2);
    
    int cnt = read(p1[0], re, 4); // 读取第二个数
    bool flag = false;
    while(cnt!=0){
        flag = true; //需要创建子进程
        // printf("进入while循环了\n");
        // printf("re: %d\n",*re);
        if(*re % n != 0){
            // 不能整除传递给子进程
            write(p2[1], re, 4);
        }
        cnt = read(p1[0], re, 4); // 读取下一个数
    }
    // 当前进程读取完左端传过来的值。且将该写的值都写给右端。
    int pid = fork();
    
    if(!flag){
        return;
    }

    if(pid==0){
        serie(p2);
        exit(0);
    }else{
        close(p2[1]);
        close(p2[0]);
        close(p1[0]);
        // int cpid = getpid();
        // printf("当前执行的是PID=%d", cpid);
        wait((int *) 0);
        exit(0);
    }
}



// void serie(int p1[]){
//     close(p1[1]); //先关掉写端。
//     int *re = malloc(sizeof(int));
//     read(p1[0], re, 4); // 读取第一个数
//     int n = *re;
//     printf("prime %d\n",n);
//     int p2[2];
//     pipe(p2);
//     int pid = -1;
    
//     read(p1[0], re, 4); // 读取第二个数
//     while(re!=0){
//         if(pid==-1){
//             pid = fork();
//         }
//         if(pid!=0){
//             // 父进程
//             if(*re % n != 0){
//                 // 不能整除传递给子进程
//                 write(p2[1], re, 4);
//             }
//             read(p1[0], re, 4); // 读取下一个数
//         }else{
//             //子进程
//             serie(p2);
//         }
//     }
//     // 当前进程读取完左端传过来的值。
//     close(p2[1]);
//     close(p2[0]);
//     close(p1[0]);
//     int cpid = getpid();
//     printf("当前执行的是PID=%d", cpid);
//     wait((int *) 0);
//     exit(0);
// }