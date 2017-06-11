
#include "include.h"


extern void yylex();

int get_current_dir_name();//获得当前路径
struct passwd *user;//存取当前用户信息

char *current_dir;//获得当前路径
char buf[BUFSIZ];//BUFSIZ为系统默认的缓冲区大小。
char* input_start;//输入的字符串的首指针
char* input_end;//输入的字符串的末指针
char lastdir[100];//当前目录

char* argbuf[200];//储存输入参数
int arg_count = 0;//储存的次数




//-------------------------------设置提示符-------------------------------

void set_prompt(char* prompt){
	

	current_dir=getcwd(NULL,100);//获取当前路径

	user = getpwuid(getuid());//获得当前用户名

	sprintf(prompt,"%s@%s:~$ ",user->pw_name,current_dir);//显示提示符,用到了pwd.h
}




//-------------------------------记录当前地址-------------------------------

void init_lastdir()
{
	getcwd(lastdir, 99);//获取当前目录
}




//-------------------------------历史命令操作-------------------------------

void history_setup()//读取历史命令
{
	using_history();

	stifle_history(50);

	read_history("/tmp/shell_history");	
}

void history_finish()//存入历史命令
{
	append_history(history_length, "/tmp/shell_history");

	history_truncate_file("/tmp/shell_history", history_max_entries);
}

void display_history()//显示历史命令
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
	//signal(SIGINT, SIG_IGN);
	
	//printf("%s",prompt);
	init_lastdir();//初始化lastdir，记录当前路径

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
   		
  	history_finish();//存入历史命令
	
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
	display_history();	
	return 0;
}



//-------------------------------改变目录-------------------------------


int shell_cd(int argc, char** argv)
{
	char* dir;
	char cwd[100];
	

	if(argc == 1) {
		if(!(dir = getenv("HOME"))) {
			printf("cd: %s\n", strerror(errno));
			return -1;
		}

	} else if(argc == 2) {
		if(strcmp(argv[1], "-") == 0) {
			dir = lastdir;
		} else if(strcmp(argv[1], "~") == 0) {
			if(!(dir = getenv("HOME"))) {
				printf("cd: %s\n", strerror(errno));
				return -1;
			}
		} else
			dir = argv[1];
	} else {
		printf("Usage: cd [dir]\n");
		return -1;
	}

	getcwd(cwd, 99); 

	if(chdir(dir) == -1) {
		printf("cd: %s\n", strerror(errno));
		return -1;
	}

	strcpy(lastdir, cwd);
	return 0;
}





//-------------------------------回显-------------------------------


 int shell_echo(int argc, char** argv)
{
	int i = 1;
	int j;
	int argn = 0;
	int arge = 0;

	if(argv[1]) {

		if(strcmp(argv[1], "-n") == 0) {
			argn = 1;
			i = 2;
		} else if(strcmp(argv[1], "-e") == 0) {
			arge = 1;
			i = 2;
		} else if((strcmp(argv[1], "-ne") == 0) || (strcmp(argv[1], "-en") == 0)) {
			argn = arge = 1;
			i = 2;
		}
	}

	j = i;
	while(argv[i]) {

		if(i > j)
			printf(" %s", argv[i]);
		else 
			printf("%s", argv[i]);
		i++;
	}

	if(argn == 0)
		printf("\n");
	
	return 0;
}


 
//-------------------------------导入或显示环境变量-------------------------------

int shell_export(int argc, char** argv)
{
	int i = 1;
	char* p;

	while(argv[i]) {

		if((p = strchr(argv[i], '='))) {
			*p = 0;

			if(strpbrk(argv[i], "~`!@#$%^&*()-_+=|\\{}[];:'\"<>,.?/")) {
				*p = '=';
				printf("export: %s: not a valid indentifier\n", argv[i]);
				i++;

				continue;
			} 

			if(setenv(argv[i], p+1, 1) == -1) 
				printf("export: %s\n", strerror(errno));
			*p = '=';
		}

		i++;		
	}
	return 0;
}




//-------------------------------清空参数-------------------------------
 void reset_args()
{
	int i;
	for(i = 0; i < arg_count; i++) {

		free(argbuf[i]);
		argbuf[i] = 0;
	}

	arg_count = 0;
}





//-------------------------------退出shell解释器-------------------------------

int shell_exit(int argc, char** argv)
{
	int val = 0;
	if(argc > 1)
		val = atoi(argv[1]);
	reset_args();
	history_finish();
	exit(val);
	return 0;
}










//----------------------------定义CMD_ENTRY结构体----------------------------------


typedef int (*cmd_handle)(int, char**);

//定义一个返回值为int，参数为int型与char**型的函数指针类型

typedef struct
{
	const char* cmd;

	cmd_handle handle;

} CMD_ENTRY;

//定义CMD_ENTRY结构体，对应操作名与其操作行为函数

const CMD_ENTRY cmd_table[] = 
{
	{"exit", 	shell_exit},
	{"cd", 		shell_cd},
	{"echo", 	shell_echo},
	{"export", 	shell_export},
	{"history", shell_history},
	{0, 0}
};




//----------------------------返回内部命令函数的指针----------------------------------


cmd_handle get_handle(const char* cmd)
{
	int i = 0;

	while(cmd_table[i].cmd) {

		if(strcmp(cmd_table[i].cmd, cmd) == 0)
			return cmd_table[i].handle;
			//当输入的命令与某操作名匹配时返回它的操作
		i++;
	}
	return 0;
}




//----------------------------记录符号参数----------------------------------

 void add_symbolArg(const char* arg)
{
	
	argbuf[arg_count] = (char*)malloc(strlen(arg)+1);
	//给argbuf分配空间
	
	strcpy(argbuf[arg_count], arg);
	//将arg字符串复制到argbuf中
	
	arg_count++;
	/////argbuf[arg_count] = 0;

}



//----------------------------记录普通参数----------------------------------

 void add_arg(const char* inputArg)
{
	char* arg;
	char buf[200];
	char buf2[200];

	int i,j,k;
	int len = strlen(inputArg);
	
	k = 0;

	for(i = 0; i < len; i++) {
		
		if(inputArg[i] == '$') {

			if(inputArg[i+1] == '$') {
			//$$,获取shell当前的进程ID号
			
				int pid = getpid();
			
				sprintf(buf+k, "%d", pid);
				
				k = strlen(buf);
				i++;

			} else if(inputArg[i+1] == '0'){
				//$0获得shell文件名
				sprintf(buf+k, "shell");
				k=k+5;
				break;

			} else {//获得$后的环境变量

				for(j = i+1; j < len; j++) {
					if(inputArg[j] == '$') {
						break;
					}
					buf2[j-i-1] = inputArg[j];
					//获得$后的环境变量名
				}

				buf2[j-i-1] = 0;
				i = j-1;

				if((arg = getenv(buf2))) {
					//获得环境变量的值
					strcpy(buf+k, arg);
					k += strlen(arg);
				}else{
					char* arg2;
					arg2 = "输入错误，请检查环境变量名" ;
					//当要获取的的环境变量名错误时给出提示
					strcpy(buf+k, arg2);
					k += strlen(arg2);
				}
			}
			
		} else {
			buf[k] = inputArg[i];
			k++; 
		}
	}

	buf[k] = 0;//结尾加上结束符'\0'

	if(k > 0){

		argbuf[arg_count] = (char*)malloc(strlen(buf)+1);
		//给buf分配空间
	
		strcpy(argbuf[arg_count],buf);
		//将buf字符串复制到argbuf中
	
		arg_count++;
	}
}




//----------------------------检查外部命令的文件是否存在----------------------------------


 int file_exist(const char* file, char* buffer)
{
	int i = 0;
	const char* p;

	//const char* path;

	p = getenv("PATH");
	//p = path;

	while(*p != 0) {

		if(*p != ':')
			buffer[i++] = *p;

		else {

			buffer[i++] = '/';//路径补充斜杠后面好加文件名
			buffer[i] = 0;
			strcat(buffer, file);//将执行程序文件的最终路径补充完整
			if(access(buffer, F_OK) == 0)//如果该文件存在返回1
				return 1;
			i = 0;//每一个路径检查完i归零，再开始记录下一个路径地址循环
		}
		p++;
	}
	return 0;
}





//-------------------------------重定向-------------------------------



int shell_redirect(int argc, char** argv, int* re)
{


	int i;
	int redirect = 0;//重定向类型，0表不存在重定向，1表示<输入重定向，2表示>输出重定向


	//先判断该段简单命令中是否存在重定向，以及是什么类型的重定向
	for(i = 1; i < argc; i++) {

		if(strcmp(argv[i], "<") == 0) {

			redirect = 1;//1表示<输入重定向
			argv[i] = 0;//2表示>输出重定向
			break;

		} else if(strcmp(argv[i], ">") == 0) {

			redirect = 2;//重定向类型2
			argv[i] = 0;
			break;	
		}
	}//for结束时确定重定向类型，当前的i即为重定向符号在字符串中的位置



	//若存在重定向
	if(redirect) {

		if(argv[i+1]) {

			int fd;

			if(redirect == 2) {//为>输出重定向时

				fd = open(argv[i+1], O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);//

				if( fd == -1) {

					fprintf(stderr, "%s或%s:不是有效文件名或不是有效指令\n", argv[i-1],argv[i+1]);
					return 1;
				}
				dup2(fd, STDOUT_FILENO);//把输出重定向到fd标识的文件
				close(fd);

			} else {//为<输入重定向时
					
					fd = open(argv[i+1], O_RDONLY, S_IRUSR|S_IWUSR);
					
					if(fd == -1) {

						fprintf(stderr, "%s或%s:不是有效文件名或不是有效指令\n", argv[i-1],argv[i+1]);
						return 1;
				}
				dup2(fd, STDIN_FILENO);//把输入重定向到fd标识的文件
				close(fd);
			}
		} else {//重定向符号后为空

			fprintf(stderr, "重定位符后 请输入有效文件名或有效指令\n");
			return 1;
		}
	}


	if(re)
		*re = redirect;

	return 0;
}





//-----------------------------执行简单命令---------------------------------



 int shell_simpleCmd(int argc, char** argv, int prefd[], int postfd[])
 {												

	int pid;
	int status;
	int oldStdout = dup(STDOUT_FILENO);		
	int oldStdin = dup(STDIN_FILENO);
	cmd_handle hd;

	if(argc == 0)//传入参数为0时直接返回
		return 0;

	//无管道时
	if(prefd == 0 && postfd == 0) {
		
		hd = get_handle(argv[0]);
		
		if(hd) //判断是否为内部命令
		{

			if(shell_redirect(argc, argv, 0))//运行重定向函数，并判断是否正常运行
				return 1;

			//执行该内部命令
			(*hd)(argc, argv);

			//恢复默认stdin、stdout
			dup2(oldStdin, STDIN_FILENO);
			dup2(oldStdout, STDOUT_FILENO);
			return 0;
		}
	}
	
	//有管道时
	if((pid = fork()) == 0) {//子进程
		

		int redirect = 0;
		//signal(SIGINT, SIG_DFL);

		if(shell_redirect(argc, argv, &redirect))//运行重定向函数，并判断是否正常运行
			exit(1);
		

		////////////修改stdin与stdout

		if(redirect != 1 && prefd) {//该段简单命令存在前管道且不存在重定向输入

			close(prefd[1]);
			
			if(prefd[0] != STDIN_FILENO) {
			//当前标准输入不在管道的读取端时

				dup2(prefd[0], STDIN_FILENO);

				close(prefd[0]);
			}
		}

		else if(redirect == 1 && prefd) {

			printf("前管道后不能接重定向输入\n");
			exit(0);
		}
		
		else if(redirect != 2 && postfd) {//该段简单命令存在后管道且不存在重定向输出
			close(postfd[0]);

			if(postfd[1] != STDOUT_FILENO) {
			//当前标准输出不在管道的写入端时	

				dup2(postfd[1], STDOUT_FILENO);

				close(postfd[1]);
			}
		}

		else if(redirect == 2 && postfd) {

			printf("后管道前不能接重定向输出\n");
			exit(0);
		}

		//////////



		if((hd = get_handle(argv[0]))) {

			(*hd)(argc, argv);
			exit(0);
		}

		char buffer[100];

		if(file_exist(argv[0], buffer)) {//判断输入指令是否存在相应的外部命令
	
			execv(buffer, argv);//执行外部命令
		}
		else {
			fprintf(stderr, "%s:该命令不存在，请检查输入是否正确\n", argv[0]);
			exit(0);
		}
    
    }

	waitpid(pid, &status, 0);

	if(postfd) { 
		close(postfd[1]); //关闭后管道
	}

	//恢复默认stdin、stdout
	dup2(oldStdin, STDIN_FILENO);
	dup2(oldStdout, STDOUT_FILENO);

	return 0;
}





//-----------------------------执行管道命令---------------------------------


 
 int shell_pipeCmd(int argc, char** argv)
{

	int i = 0;
	int j = 0;
	char* p;

	int prepipe = 0; //保存前管道是否存在的状态
	
	//保存一段命令的前后管道得读写端口数组
	int prefd[2];
	int postfd[2];

	
	while(argv[i]) {

		if(strcmp(argv[i], "|") == 0) { //检测到管道符号

			p = argv[i];//备份该处得指针
			argv[i] = 0;//在原分号处改为结束符号'\0'，将该段字符串结束

			pipe(postfd); 		//建立管道
			
			
			if(prepipe)	//在第一个管道时无前管道，第二个管道时才执行这个if得内容
				shell_simpleCmd(i-j, argv+j, prefd, postfd);
				//当前管道存在时,将各个参数传给简单命令函数处理

			else
				shell_simpleCmd(i-j, argv+j, 0, postfd);
				//当前管道存在时,将各个参数传给简单命令函数处理


			argv[i] = p;//复原argv

			prepipe = 1;//检测到一个管道时便将其设置为1表存在，为下次if判断准备

			prefd[0] = postfd[0];//将前一段命令的后管道读取端传递给后一段命令的前管道读取端
			prefd[1] = postfd[1];//将前一段命令的后管道写入端传递给后一段命令的前管道写入端
			
			j = ++i;//准备下一次循环


		} else
			i++;

	}

	if(prepipe)//当有管道存在时，最后一段命令无法进入上面的if判断‘|’中，在此执行
		shell_simpleCmd(i-j, argv+j, prefd, 0);

	else //无管道存在时，直接传递给下一函数执行
		shell_simpleCmd(i-j, argv+j, 0, 0);
	return 0;
}






//---------------------------执行列表命令----------------------------------



void shell_listCmd()
{
	int i = 0;
	int j = 0;
	char* p;

	while(argbuf[i]) {
	
		if(strcmp(argbuf[i], ";") == 0) {//	 ;
	
			p = argbuf[i];//备份该处得指针

			argbuf[i] = 0;//在原分号处改为结束符号'\0'，将该段字符串结束
			shell_pipeCmd(i-j, argbuf+j);//将该段参数内容与其长度传给管道函数处理

			argbuf[i] = p;//复原argbuf
			j = ++i;//准备下一次循环
	
		} else
			i++;
	}

	shell_pipeCmd(i-j, argbuf+j);//执行最后那段无分号结尾的命令
}

 







