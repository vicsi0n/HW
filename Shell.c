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

extern void yylex();
int get_current_dir_name();//获得当前路径
struct passwd *user;//存取当前用户信息
char *current_dir;//获得当前路径
char buf[BUFSIZ];//BUFSIZ为系统默认的缓冲区大小。
char* input_start;
char* input_end;
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


//-------------------------------历史命令操作-------------------------------

void history_setup()//读取历史命令
{
	using_history();
	stifle_history(50);
	read_history("/tmp/shell_history");	
}

void history_finish()
{
	append_history(history_length, "/tmp/shell_history");
	history_truncate_file("/tmp/shell_history", history_max_entries);
}

void display_history_list()//显示历史命令
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
	
	char* input;
	char prompt[200];
	signal(SIGINT, SIG_IGN);
	
	//printf("%s",prompt);
	init_lastdir();
	history_setup();	
	while(1)
  {

  	set_prompt(prompt);//创建提示符
	input = readline(prompt);//显示提示符并读取输入

 		if (!input)//检测EOF（-1）终止程序
 		{
 			break; 
 		}
 		else if (*input)
 		{
 			add_history(input);//将输入添加到历史命令记录
 		}
 		strcpy(buf,input);//将输入传给buf
 		strcat(buf,"\n");

 		//free(input);

 		input_start = buf;
		input_end = buf+strlen(buf);
		//printf("=%s+\n",input_start);

	yylex();//进行词法分析
  }
   		
  	history_finish();
	
	return 0;
}




//-------------------------------自定义lex的YY_INPUT-------------------------------

 int YYinput(char* buf, int max)
{
	int n;
	n = (max < (input_end-input_start)) ? max : (input_end-input_start);

	if(n > 0) {
		memcpy(buf, input_start, n);

		input_start += n;
	}
	return n;
}



//-------------------------------显示命令历史信息-------------------------------


int shell_history(int argc, char** argv)
{
	display_history_list();	
	return 0;
}

//-------------------------------退出shell解释器-------------------------------

 int shell_exit(int argc, char** argv)
{

}



//-------------------------------改变目录-------------------------------


 int shell_cd(int argc, char** argv)
{
	
}



//-------------------------------导入或显示环境变量-------------------------------


 int shell_export(int argc, char** argv)
{
	
}



//-------------------------------回显-------------------------------


 int shell_echo(int argc, char** argv)
{
	
}




 


