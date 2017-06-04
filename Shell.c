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

#include <readline/readline.h>
#include <readline/history.h>

extern void yylex();
int get_current_dir_name();//获得当前路径
struct passwd *user;//存取当前用户信息
char *current_dir;//获得当前路径
char buf[BUFSIZ];//BUFSIZ为系统默认的缓冲区大小。
char* myptr;
char* mylim;
char lastdir[100];



//-------------------------------设置提示符-------------------------------

void set_prompt(char* prompt){
	

	current_dir=getcwd(NULL,100);//获取当前路径

	user = getpwuid(getuid());//获得当前用户名

	sprintf(prompt,"%s@%s:~$ ",user->pw_name,current_dir);//显示提示符,用到了pwd.h
}

//-------------------------------记录当前地址-------------------------------

void init_lastdir()
{
	getcwd(lastdir, 99);
}


//-------------------------------存入历史命令-------------------------------

void history_setup()
{
	using_history();
	stifle_history(50);
	read_history("/tmp/msh_history");	
}

void history_finish()
{
	append_history(history_length, "/tmp/msh_history");
	history_truncate_file("/tmp/msh_history", history_max_entries);
}

void display_history_list()
{
	HIST_ENTRY** h = history_list();
	if(h) {
		int i = 0;
		while(h[i]) {
			printf("%d: %s\n", i, h[i]->line);
			i++;
		}
	}
}



//-------------------------------主函数-------------------------------

int main()
{
	
	char* line;
	char prompt[200];
	signal(SIGINT, SIG_IGN);
	
	//printf("%s",prompt);
	init_lastdir();
	history_setup();	
	while(1)
  {

  	set_prompt(prompt);//创建提示符
	line = readline(prompt);//读取提示符
 		if (!line)
 		{
 			break; 
 		}
 		else if (*line)
 		{
 			add_history(line);
 		}
 		strcpy(buf,line);
 		strcat(buf,"\n");

 		myptr = buf;
		mylim = buf+strlen(buf);
	yylex();//进行词法分析
  }
   		
  	history_finish();
	
	return 0;
}




