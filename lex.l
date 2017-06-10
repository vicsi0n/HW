
%{

	
#include "include.h"



extern int YYinput(char* buf, int max);

#undef 	YY_INPUT
#define YY_INPUT(buf,result,max_size) (result = YYinput(buf,max_size))


extern void add_arg(const char* xarg);
extern void add_symbolArg(const char* xarg);
extern void reset_args();
extern void shell_listCmd();




%}

%option nounput
%option noinput

%x		QUOTE    

%%

"\""			{BEGIN QUOTE;}              /* 检测到引导，进入条件匹配状态 */
<QUOTE>[^\n"]+	{add_arg(yytext);}			/* 满足QUOTE条件，当输入的非换行键时，执行add_arg(yytext)，把整个字符串加入到参数数组中 */
<QUOTE>"\""		{BEGIN 0;}					/* 满足QUOTE条件，当输入引号时，退出条件匹配状态*/
<QUOTE>\n		{BEGIN 0; shell_listCmd(); reset_args();} /* 满足QUOTE条件，当输入换行键时，结束了本次扫描，退出条件匹配状态，并执行shell_list_cmd(); reset_args(); */

";"				{add_symbolArg(yytext);}	/* 检测到分号，这是一个列表命令结束的操作符，所以作为一个单独的参数，执行add_simple_arg(yytext) */
">"				{add_symbolArg(yytext);}   /* 检测到大于号，这是一个简单命令结束的操作符，所以作为一个单独的参数，执行add_simple_arg(yytext) */
"<"				{add_symbolArg(yytext);}   /* 检测到小于号，这是一个简单命令结束的操作符，所以作为一个单独的参数，执行add_simple_arg(yytext) */
"|"				{add_symbolArg(yytext);}   /* 检测到|，这是一个管道命令结束的操作符，所以作为一个单独的参数，执行add_simple_arg(yytext) */
[^ \t\n|<>;"]+	{add_arg(yytext);}			/* 检测到非tab键、回车键、|、<、>、;等符号 且至少匹配到1次时，作为一个普通的参数，加入参数数组， 执行add_arg(yytext) */
\n				{shell_listCmd(); reset_args();} /* 检测到回车键输入时，对列表命令的处理，执行shell_list_cmd(); reset_args(); */
.				;   /* 忽略其它任意字符 */

%%












