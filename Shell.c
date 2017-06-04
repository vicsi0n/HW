#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <pwd.h>

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



//-------------------------------set_prompt-------------------------------

void set_prompt(char* prompt){
	

	current_dir=getcwd(NULL,100);//获取当前路径

	user = getpwuid(getuid());//获得当前用户名

	sprintf(prompt,"%s@%s:~$ ",user->pw_name,current_dir);//显示提示符,用到了pwd.h
}

//-------------------------------init_lastdir-------------------------------

void init_lastdir()
{
	getcwd(lastdir, 99);
}


//-------------------------------history-------------------------------

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



//-------------------------------main-------------------------------

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
	line = readline(prompt);
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
	yylex();
  }
   		
  	history_finish();
	
	return 0;
}




