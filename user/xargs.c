#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"


void
printASCII(const char *str)
{
    printf("ASCII codes for string \"%s\":\n", str);
    for (int i = 0; str[i] != '\0'; ++i) {
        printf("%c: %d\n", str[i], str[i]);
    }
}


char*
getline(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    // printf("cc: %c\n", c);
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

char buf[512];
#define NULL ((void*)0)

int
append_args(int argc, char *argv[],  char *tokens[], int token_count) 
{
    for (int i=0; i<token_count;i++){
      argv[argc++] = tokens[i];
    }
    return argc;
}

int
split_string(char *str, char *delim, char **tokens)
{
    int token_count = 0;
    char *token;
    
    // 使用strchr()查找字符串中的分隔符
    while ((token = strchr(str, *delim)) != NULL) {
        *token = '\0'; // 将分隔符替换为字符串结束符
        tokens[token_count++] = str; // 存储子字符串的指针到数组中
        printf("split-str: %s\n", str);
        str = token + 1; // 移动字符串指针到下一个位置
    }
    /**
     * 之前一直有疑惑，上面存储指针str到tokens的时候，每一次循环，存储一次。
     * 如果是高级语言,str是一个对象，而循环内只让str的引用指向了不同内存对象。因此循环到最后，只会得到全是str元素的数组，且str指向了最后一次指向的内存对象。
     * 但这里是C语言，str本身是字符指针（其实就是一串无符号整数），循环内会 str = token + 1，将会直接改变str的值。
     * tokens[token_count++] = str; 这一句是将str包含的地址值直接给到tokens数组。所以不会出现高级语言的情况。
    */
    token = strchr(str, '\n');
    *token = '\0'; //得修改，否则会多打一个空行。
    tokens[token_count++] = str; // 存储最后一个子字符串的指针到数组中
    // printf("split-str: %s\n", str);
    return token_count;
}

void 
xarg(int argc, char *argv[]){
    // 从标准输入循环读取
    
    while (1) {
        getline(buf, sizeof(buf));
        // printf(">>> gets:%s", buf);
        // printASCII(buf);
        if(buf[0]=='\0')
        {
          break;
        }
        char *tokens[MAXARG]; // 指针数组，存储解析后的附加参数。

        // 使用动态内存分配来存储子字符串的指针数组 ; 解析附加参数。最大附加参数应为 MAXARG-argc
        for (int i = 0; i < MAXARG-argc; i++) { 
            tokens[i] = malloc(512 * sizeof(char)); // 分配内存 每个字符串参数最大字符数量512
        }
        // printf(">>> gets:%s", buf);

        int token_count = split_string(buf, " ", tokens); //解析附加参数。

        if(token_count>MAXARG-argc){
          printf("too many args!");
          exit(1);
        }
        /**
         * echo hello
         * +
         * word1 word2 word2
         * word1 word2
         * 
        */
        // printf(">>>> split end\n");
        char *exec_args[MAXARG];
        int k=0;
        for(int i=0;i<argc;i++){
          exec_args[k++] = argv[i];
        }
        for(int i=0;i<token_count;i++){
          exec_args[k++] = tokens[i];
        }

       
        

        // argc = append_args(old_argc,argv,tokens,token_count);

        
        // for (int i = 0; i < k; i++) {
        //     printf("arg[%d]: %s\n",i,exec_args[i]);
        // }
        // printf(">>> gets:%s", buf);
        int pid = fork();
        if(pid != 0){
          //父进程
          wait((int *) 0);
        }else{
          //子进程
          exec(exec_args[0], exec_args);
        }
    }
}


int
main(int argc, char *argv[])
{

  if(argc < 2){
    // find(".");
    printf("invalid arguments");
    exit(1);
  }
  // printf("arguments: %s, %s \n", argv[1],argv[2]);
  // find(argv[1], argv[2]);
  // 此处要获取xargs指令的第1个参数（第0个为xargs本身），作为新指令的参数0。因此等价于将argv[]整体向左移一个元素。
  // 该操作只需要做一次。
  for (int i = 1; i < argc; i++) {
      argv[i - 1] = argv[i]; 
  }
  xarg(argc-1, argv);
  exit(0);
}