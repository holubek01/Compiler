%option noyywrap
%option nounput
%option yylineno

%{
#include <iostream> 
#include "parser.hpp"
#include "mylib.hpp" 
using namespace std;

bool in_proc_declaration = false;
char* last_calling_proc_name;
int args_num = 0;
int args_call_num = 0;
bool is_in_loop = false;
int com_depth = 0;
int line=1;
int iteration=0;

%}
%x find_variables find_procedure find_procedure_args inside_com

identifier [_a-z]+

%%
         
[ \r\t]+
[\n]              {line++;}

"["                 { com_depth++; BEGIN(inside_com);}
<inside_com>"["     {com_depth++; if(com_depth>1){cerr<<endl<<"Błąd: Zagnieżdżony komentarz w linii "<<line<<endl; exit(0);}; }
<inside_com>\n      {line++; BEGIN(inside_com);}
<inside_com>[^\[\]\n]* { BEGIN(inside_com);}
<inside_com>"]"     {com_depth--; BEGIN(INITIAL);}


"PROGRAM" 	    {set_start_point(&iteration); return PROGRAM;}                     
"PROCEDURE"     { BEGIN(find_procedure); return PROCEDURE; }
"IS"            { return IS;}
"VAR"           { BEGIN(find_variables); return VAR;}
"BEGIN"         { return TBEGIN;}
"END"           { return END; }
"IF"            { return IF; }
"THEN"          { return THEN; }
"ELSE"          { return ELSE; }
"ENDIF"         { return ENDIF; }
"WHILE"         { is_in_loop = true; return WHILE; }
"DO"            { return DO; }
"ENDWHILE"      { is_in_loop = false; return ENDWHILE; }
"REPEAT"        { is_in_loop=true; return REPEAT; }
"UNTIL"         { is_in_loop=false; return UNTIL; }
"READ"          { return READ; }
"WRITE"         { return WRITE; }
":="            { return ASSIGN; }            
"+"             { return ADD; }
"-"             { return SUB; }
"*"             { return MUL; }
"/"             { return DIV; }
"%"             { return MOD; }
"="             { return EQ; }
"!="            { return NEQ; }
"<"             { return LT; }
">"             { return GT; }
"<="            { return LEQ; }
">="            { return GEQ; }
","             { return COMA; }
"("             {  return LBR; }
")"             { if(in_proc_declaration){ if(!is_argsnum_matched(args_call_num, last_calling_proc_name))
                { cerr <<endl<< "Błąd: Procedura " <<last_calling_proc_name<< " wywołana ze złą liczbą arguemntów w linii " << line <<endl;; 
                exit(1); }; } in_proc_declaration=false; args_call_num=0; last_calling_proc_name = ""; return RBR; }

":"             { return COL; }
";"             { return SEM; }

<find_procedure>{identifier}    { if(get_symbol_index(yytext,0, "PROC") < 0) {yylval.val=(int)add_proc_to_table(yytext, &iteration); return IDENTIFIER;} 
                                else { cerr <<endl<< "Błąd: Druga deklaracja procedury "<<yytext<<" w programie w linii " << line << endl; exit(1); };}

<find_procedure>"("             {BEGIN(find_procedure_args); return LBR;};

<find_procedure_args>{identifier}	{if(get_symbol_index(yytext, iteration, "VARIABLE") < 0) { yylval.val=add_var_to_table(yytext); args_num+=1; initialize(yylval.val-1); return IDENTIFIER;} 
                                        else { cerr <<endl<< "Błąd: Druga deklaracja zmiennej "<<yytext<<" w linii " << line<<endl;  exit(1); } ;}

<find_procedure_args>","		    {BEGIN(find_procedure_args); return COMA;}
<find_procedure_args>")"		    {add_args_to_proc(args_num); args_num = 0; BEGIN(INITIAL); return RBR;}


<find_variables>{identifier}  {if(get_symbol_index(yytext, iteration, "VARIABLE") < 0) { yylval.val=add_var_to_table(yytext); return IDENTIFIER;} else 
                                { cerr <<endl<< "Błąd: Druga deklaracja zmiennej "<<yytext<<" w linii " << line<<endl;  exit(1); } }
<find_variables>","		{BEGIN(find_variables); return COMA;}
<find_variables>\n      {line++;}
<find_variables>"BEGIN"		{BEGIN(INITIAL); return TBEGIN;}


{identifier}"("               { int helper; if( (helper=is_declared_proc(yytext, iteration)) < 0) { yytext[strlen(yytext)-1]='\0'; 
                            cerr <<endl<< "Błąd: Uźycie nie zadeklarowanej procedury "<<yytext<<" w linii "<<line<<endl; exit(1); } 
                            else { yytext[strlen(yytext)-1]='\0'; yylval.val=helper;in_proc_declaration = true; last_calling_proc_name = yytext; return CALL;  } }


{identifier}               { int helper; if( (helper=get_symbol_index(yytext, iteration, "VARIABLE")) < 0)
                        { cerr <<endl<< "Błąd: Użycie nie zadeklarowanej zmiennej "<<yytext<<" w linii "<<line<<endl;
                        exit(1); } else { yylval.val=helper; 
                        if(in_proc_declaration){args_call_num+=1; initialize(yylval.val);} return IDENTIFIER; } }

[0-9]+                      {yylval.val=add_constant(yytext); return NUM; }
.                           { cerr <<endl <<"Błąd: Nieznany symbol: " << yytext << " w linii" << line<<endl; exit(1);}
%%
