#include <stdio.h>//C语言输入输出库
#include <unistd.h>//类UNIX操作系统POSIX的API原句
#include <string.h>//C语言字符串操作库
#include <fcntl.h>//文件操作
#include <stdlib.h>

#include <sys/wait.h>//调用系统的阻塞等待函数
#include <signal.h>//信号处理
#include <pwd.h>//提供了passwd这个用户基本信息的结构体

#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <readline/readline.h>
#include <readline/history.h>