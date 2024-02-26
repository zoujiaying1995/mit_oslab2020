#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int
main(int argc, char *argv[])
{
  if(argc<2||argc>3){
    printf("args err: you should pass only 1 argument. \n");
    exit(1);
  }
  char *arg1 = argv[1];
  int t = atoi(arg1);
  if(sleep(t)<0){
    printf("something wrong. \n");
    exit(1);
  }
  exit(0);
}
