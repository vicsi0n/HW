%{
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>


extern int YYinput(char* buf, int max);
#undef 	YY_INPUT
#define YY_INPUT(buf,result,max_size) (result = YYinput(buf,max_size))


extern int shell_exit(int, char**);
extern int shell_export(int, char**);
extern int shell_echo(int, char**);
extern int shell_cd(int, char**);
extern int shell_history(int, char**);



typedef int (*command_handle)(int, char**);
typedef struct
{
	const char* cmd;
	command_handle handle;
} DO_COMMAND;

const DO_COMMAND cmd_list[] = 
{
	{"exit", 	shell_exit},
	{"cd", 		shell_cd},
	{"echo", 	shell_echo},
	{"export", 	shell_export},
	{"history", shell_history},
	{0, 0}
};

%}


%x		QUOTE

%%

"\""			{BEGIN QUOTE;}
<QUOTE>[^\n"]+	{}
<QUOTE>"\""		{BEGIN 0;}
<QUOTE>\n		{BEGIN 0;;}
";"				{;}
">"				{;}
"<"				{;}
"|"				{;}
[^ \t\n|<>;"]+	{;}
\n				{;}
.				;

%%






















