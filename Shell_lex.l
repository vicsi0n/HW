%{

	
#include "include.h"


extern void display_history_list();
extern void history_finish();

extern int YYinput(char* buf, int max);




extern void add_arg(const char* xarg);
extern void add_simple_arg(const char* xarg);
extern void reset_args();
extern void shell_list_cmd();


#undef 	YY_INPUT
#define YY_INPUT(buf,result,max_size) (result = YYinput(buf,max_size))






%}

%x		QUOTE    

%%

"\""			{BEGIN QUOTE;}               /* xxx */
<QUOTE>[^\n"]+	{add_arg(yytext);}
<QUOTE>"\""		{BEGIN 0;}
<QUOTE>\n		{BEGIN 0; shell_list_cmd(); reset_args();}

";"				{add_simple_arg(yytext);}
">"				{add_simple_arg(yytext);}
"<"				{add_simple_arg(yytext);}
"|"				{add_simple_arg(yytext);}
[^ \t\n|<>;"]+	{add_arg(yytext);}
\n				{shell_list_cmd(); reset_args();}
.				;

%%

 











