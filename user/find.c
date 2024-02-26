#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), '\0', DIRSIZ-strlen(p));
  return buf;
}

void
find(char *path, char *target)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  // printf("进入Find, %s, %s \n", path, target);
  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    // printf("类型为T——File, %s\n",path);
    // printf("类型为T——File1, %s\n",fmtname(path));
    // printf("类型为T——File2, %s\n",target);
    if(strcmp(fmtname(path), target)==0){
      printf("%s\n",path);
    }
    // printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    // printf("类型为T——Dir, %s\n",path);
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      // printf("进入while循环, %s\n",path);
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0; // 在将文件名拷贝到 buf 中之后，为了确保文件名的结尾，需要在文件名后面添加一个空字符（'\0'），以表示字符串的结束。这样做可以保证在打印文件名时，不会输出不属于文件名的内容。
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      
      if(strcmp(fmtname(buf),".")==0||strcmp(fmtname(buf),"..")==0){
        // printf("进入continue, %s, %s %d %d %d\n", buf, fmtname(buf), st.type, st.ino, st.size);
        continue;
      }
      
      find(buf, target);
      // printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}


void printASCII(const char *str) {
    printf("ASCII codes for string \"%s\":\n", str);
    for (int i = 0; str[i] != '\0'; ++i) {
        printf("%c: %d\n", str[i], str[i]);
    }
}

// void trim(char *str) {
//     char *start = str;
//     char *end = str + strlen(str) - 1;

//     // 找到第一个非空格字符
//     while (isspace(*start)) {
//         start++;
//     }

//     // 找到最后一个非空格字符
//     while (isspace(*end) && end >= start) {
//         end--;
//     }

//     // 移动字符串，去除前面的空格
//     memmove(str, start, end - start + 1);
    
//     // 在末尾加上空字符
//     str[end - start + 1] = '\0';
// }

/**
    1. 找出当前目录下的文件。
        - 如果是文件，则判断是否含有target来打印。
        - 如果是目录，则递归。

*/

int
main(int argc, char *argv[])
{

  if(argc != 3){
    // find(".");
    printf("invalid arguments");
    exit(1);
  }
  // printf("arguments: %s, %s \n", argv[1],argv[2]);
  find(argv[1], argv[2]);
  exit(0);
}