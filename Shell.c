#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>



char cmd[40]="";		
char *arg[10]={NULL};		
char addr[40]="";	
int cn=0;			
NODE *head=NULL;		
pid_t PID;			


//-------------------------------主函数-------------------------------

int main()
{
	int i=0;
	int j=0;
	int key=0;			      
	int flag=0;
	NODE *pi=NULL;
	pid_t pid;		
	getcwd(addr,40);			
	while(1)
	{
		j=0;
		printf("Shell@%s> ",get_current_dir_name());
		memset(cmd,0,40);		
		set_keypress();			//设置输入的模式
	
		while(1)				//输入命令
		{
			key=getchar();
			if(key == 27)		
			{
				history();
			}
			else if(key == 127)		
			{
				cmd[--j] = '\0';
				printf("\r");
				printf("\rShell@%s> %s",get_current_dir_name(),cmd);
			}
			else if(key == 10)	//键入Enter时，保存命令
			{
				save_cmd();
				break;
			}
			else			
			{
				printf("%c",key);
				cmd[j++]=(char)key;
			}
		}
		printf("\n");	
		reset_keypress();		//恢复标准模式
		strcpy(cmd,addBlank(cmd));	
		cn=my_strstr(cmd,arg);		//以空格分割命令行
		if(arg[0] != NULL)
		{
			if(isincmd(arg) == 1)			//执行内部命令
			{
				continue;
			}
			else if(strcmp(arg[0],"jobs") == 0)		//执行jobs命令
			{
				output_jobs();
			}
			else if(strcmp(arg[0],"fg") == 0)			//执行fg命令
			{
				fg_jobs();	
			}
			else if(strcmp(arg[0],"bg") == 0)			//执行bg命令
			{
				bg_jobs();
			}
			else
			{
				if(arg[1] != NULL)
				{
					
					if(strcmp(arg[1],"&") == 0)//在后台执行命令
					{
						flag=1;	
						pi=(NODE *)malloc(sizeof(NODE));
						pi->num=NUM;
						pi->pid=getpid();
						strcpy(pi->state,"stopped");
						strcpy(pi->comand,arg[0]);
						head=jobs_link(pi);//将此后台执行的命令添加到jobs链表中
					}
				}
				if((pid=fork()) < 0)	//创建子进程
				{
					perror("fork error!\n");
					exit (1);
				}
				if(pid == 0)
				{
					i=check();
					switch(i)
					{
						case 1:
							redirect();//执行类似"cat>text"的重定向命令
							break;
						case 2:		  //执行类似"cat>>text"的重定向命令
							redirect_cat();
							break;
						case 3:	      //执行类似"cat>test<text"的重定向命令
							redirect_a();
							break;
						case 4:	//执行类似"cat>>test<text"的重定向命令
							redirect_cat_a();
							break;
						case 5:	//执行管道命令
							my_pipe();
							break;
						default:
							execvp(arg[0],arg);//执行普通的外部命令
							break;
					}
				}
				else
				{
					PID=pid;
					if(flag == 1)	//执行后台命令，父进程不等待子进程
					{
						;
					}
					else	        //执行前台命令，父进程等待子进程
					{
						waitpid(pid,NULL,WUNTRACED);
					}
				}
			}
		}
	}
}














