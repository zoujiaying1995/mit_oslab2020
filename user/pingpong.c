#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc>2){
    printf("args err: you should pass 0 argument. \n");
    exit(1);
  }
  int p1[2];
  int p2[2];
  pipe(p1); // parent -> child ; 在数组p中， 0：读端，1：写端。 在 | 中， 左边为写，右边为读。
  pipe(p2); // child -> parent

  if(fork() == 0){
    // 子进程
    int pid = getpid();
    close(p1[1]);
    char *c = malloc(sizeof(char));
    read(p1[0], c, 1);
    printf("%d: received ping\n", pid);
    close(p1[0]);
    write(p2[1], "a", 1);
    close(p2[1]);
    close(p2[0]);
    
  }else{
    // 父进程
    int pid = getpid();
    write(p1[1], "b", 1);
    close(p1[1]);
    close(p1[0]);
    close(p2[1]); // 关闭完写端才能读取到数据结尾。
    char *c = malloc(sizeof(char));
    read(p2[0], c, 1);
    printf("%d: received pong\n", pid);
    close(p2[0]);
    
  }
  
  exit(0);
}