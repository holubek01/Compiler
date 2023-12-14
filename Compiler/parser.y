%{
#include <iostream>
#include "parser.hpp"
#include "mylib.hpp"
    
using namespace std;
    
int yylex();
void yyerror(char*);
extern int line;
extern int yylineno;
extern bool is_in_loop;

%}

%union{
    long long val; 
    struct Action* ptr;
}

%start program_all
%token PROGRAM
%token PROCEDURE
%token IS
%token VAR
%token TBEGIN
%token END
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE
%token REPEAT UNTIL
%token READ
%token WRITE 
%token COMA
%token ADD SUB MUL DIV MOD       
%token EQ NEQ LT GT LEQ GEQ
%token LBR RBR COL SEM
%token ASSIGN

%token <val> IDENTIFIER
%token <val> NUM
%token <val> CALL


%type <ptr> command
%type <ptr> commands
%type <ptr> expression
%type <ptr> condition
%type <ptr> value
%type <ptr> main
%type <ptr> procedures
%type <ptr> program_all
%type <ptr> proc_head2
%type <ptr> proc_head
%type <ptr> declarations

%%
program_all:
  procedures main								{add_start_program($1,$2);}
;

procedures
: procedures error								                                        {yyerror("blad");}
| procedures PROCEDURE proc_head IS VAR declarations TBEGIN commands END	{$$ = add_procedure($1,$3, $8);}	
| procedures PROCEDURE proc_head IS TBEGIN commands END		            	{$$ = add_procedure($1,$3, $6);}
| %empty							                                              		{$$ = add_empty(AST_PROCEDURE); }
;

main
: PROGRAM IS VAR declarations TBEGIN commands END				{$$ = add_main($6);}
| PROGRAM IS TBEGIN commands END						            {$$ = add_main($4);}
;

commands  
: %empty            								{$$ = add_empty(AST_COMMANDS);}
| commands command      							{$$ = add_action($1, $2); }  
| commands error								{yyerror("Blad"); }  
;

command
: IDENTIFIER ASSIGN expression SEM                         				{$$ = add_parent_action(AST_ASSIGN,2, add_action2(AST_IDENTIFIER, $1), $3); initialize($1);}  
| IF condition THEN commands ELSE commands ENDIF     				    {$$ = add_elseif_action(AST_IFELSE, $2, $4, $6);}     
| IF condition THEN commands ENDIF                      				{$$ = add_if_action(AST_IF, $2, $4);}  
| WHILE condition DO commands ENDWHILE                  				{$$ = add_while_action(AST_WHILE, $2, $4);}  
| REPEAT commands UNTIL condition SEM  						{$$ = add_repeat_action(AST_REPEAT, $2, $4);}  
| proc_head2                 								{$$ = add_call_action(AST_CALL, $1);}  
| READ IDENTIFIER SEM                                 				  	{$$ = add_parent_action(AST_READ,1, add_action2(AST_IDENTIFIER, $2));  initialize($2);}  
| WRITE value SEM                                      			 	{$$ = add_parent_action(AST_WRITE,1, $2); }    
;

declarations
: declarations COMA IDENTIFIER				{$$ = add_add_arg_action($1,add_action2(AST_IDENTIFIER, $3));  } 
| IDENTIFIER							            {$$ = add_empty_proc_action(AST_ARGS, add_action2(AST_IDENTIFIER, $1)); } 
;



proc_head
: IDENTIFIER LBR declarations RBR					{ $$ = add_proc_action(AST_ADD_PROCEDURE, add_action2(AST_IDENTIFIER, $1));}  
;

proc_head2									
: CALL declarations RBR SEM							{ $$ = add_proc_calling_action(AST_PROCCALLING,add_action2(AST_IDENTIFIER, $1), $2); } 
;

expression
: value                 {$$ = $1;}
| value ADD value       {$$ = add_parent_action(AST_ADD,2, $1, $3);}  
| value SUB value       {$$ = add_parent_action(AST_SUB,2, $1, $3);}   
| value MUL value       {$$ = add_parent_action(AST_MUL,2, $1, $3);}  
| value DIV value       {$$ = add_parent_action(AST_DIV,2, $1, $3);}    
| value MOD value       {$$ = add_parent_action(AST_MOD,2, $1, $3);}    
;

condition
: value EQ value        {$$ = add_parent_action(AST_EQ,2, $1, $3);}  
| value NEQ value       {$$ = add_parent_action(AST_NEQ,2, $1, $3);}  
| value LT value        {$$ = add_parent_action(AST_LT,2, $1, $3);}    
| value GT value        {$$ = add_parent_action(AST_GT,2, $1, $3);}    
| value LEQ value       {$$ = add_parent_action(AST_LEQ,2, $1, $3);}    
| value GEQ value       {$$ = add_parent_action(AST_GEQ,2, $1, $3);}    
;

value
: NUM                   {$$ = add_action2(AST_NUM, $1);}  
| IDENTIFIER            {$$ = add_action2(AST_IDENTIFIER, $1); check_initilized($1, line, is_in_loop);}  
; 

%%

void yyerror(char *err){
    cerr << endl<< "Błąd: "<<err<< "w linii " << line<<endl;
    exit(1);
}
