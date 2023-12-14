#pragma once
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wswitch"


#include <iostream>
#include <string.h>
#include <vector>
using namespace std;

enum ElementType{
    VARIABLE,
    CONSTANT,
    PROC
};


enum AbstractType{ABSTRACT_READ, ABSTRACT_WRITE, ABSTRACT_HALF, ABSTRACT_OVERWRITE, ABSTRACT_ADD, ABSTRACT_SUB, ABSTRACT_MUL, ABSTRACT_DIV, ABSTRACT_MOD, ABSTRACT_JUMP,ABSTRACT_JZERO, ABSTRACT_HALT,
    ABSTRACT_ENDIF,ABSTRACT_IF, ABSTRACT_LT,ABSTRACT_GT,ABSTRACT_LEQ,ABSTRACT_GEQ,ABSTRACT_EQ,ABSTRACT_NEQ,ABSTRACT_JUMPIF,ABSTRACT_JUMPELSE,ABSTRACT_ENDELSEIF,ABSTRACT_ELSEIF,
    ABSTRACT_WHILEBEGIN,ABSTRACT_WHILEEND,ABSTRACT_WHILEBREAK,ABSTRACT_WHILE,ABSTRACT_REPEAT,ABSTRACT_BEGINREPEAT, ABSTRACT_JUMPREPEAT,ABSTRACT_ENDREPEAT,ABSTRACT_ADD_PROCEDURE,
    ABSTRACT_PROCBLOCKSTART, ABSTRACT_PROCBLOCKEND, ABSTRACT_ENDPROCEDURE, ABSTRACT_CALL, ABSTRACT_PROCCALLING, ABSTRACT_ARGS, ABSTRACT_GIVE_PARAMS, ABSTRACT_JUMPTOPROC,
    ABSTRACT_PROCRETURN, ABSTRACT_GIVE_PARAMS_TO_PROC, ABSTRACT_GET_PARAMS_FROM_PROC, ABSTRACT_GET_PARAMS};

    
enum ActionType{AST_READ,AST_WRITE, AST_HALF, AST_OVERWRITE,  AST_ADD,AST_SUB, AST_MUL, AST_DIV,         
    AST_MOD,AST_JUMP, AST_JZERO, AST_HALT,AST_ENDIF,AST_IF, AST_LT, AST_GT, AST_LEQ, AST_GEQ,AST_EQ, AST_NEQ, AST_JUMPIF, AST_JUMPELSE,
    AST_ENDELSEIF, AST_ELSEIF, AST_WHILEBEGIN, AST_WHILEEND,AST_WHILEBREAK, AST_WHILE, AST_REPEAT, AST_BEGINREPEAT , AST_JUMPREPEAT,AST_ENDREPEAT,
    AST_ADD_PROCEDURE, AST_PROCBLOCKSTART, AST_PROCBLOCKEND, AST_ENDPROCEDURE, AST_CALL, AST_PROCCALLING, AST_ARGS, AST_GIVE_PARAMS, AST_JUMPTOPROC,
    AST_PROCRETURN,AST_GIVE_PARAMS_TO_PROC, AST_GET_PARAMS_FROM_PROC, AST_GET_PARAMS,
    AST_IDENTIFIER, AST_NUM , AST_ENDMAIN, AST_EMPTYREPEAT,
    AST_PROGRAM_ALL,AST_PROCEDURE,AST_COMMANDS,AST_ASSIGN,AST_MAIN,AST_IFELSE
};


enum AssemblerActionType{GET_ASSEMLBER, PUT_ASSEMLBER, LOAD_ASSEMLBER, STORE_ASSEMLBER, LOADI_ASSEMLBER, STOREI_ASSEMLBER, ADD_ASSEMLBER, SUB_ASSEMLBER, ADDI_ASSEMLBER, SUBI_ASSEMLBER, SET_ASSEMLBER, HALF_ASSEMLBER, JUMP_ASSEMLBER, JPOS_ASSEMLBER, JZERO_ASSEMLBER, JUMPI_ASSEMLBER, HALT_ASSEMLBER};

struct Action* check_should_resize(Action* parent);
struct Action* add_action2(ActionType typ, int i);
int is_declared_variable(string name, int iter);
void set_program_values(ActionType type, int index, int size);
void set_start_point(int* iter);
int is_declared_proc(string name, int iter);
void init(string what_to_init);
void set_action_values2(Action* ptr, ActionType type, int index, int size, int max);

void create_element(ElementType type, string name, int args_num, bool default_init);
int get_symbol_index(string name, int iter, string type);
int add_var_to_table(string name);
int add_proc_to_table(string name, int* iter);
int add_constant(string number);
int resize(string what_to_resize);
void add_arg_to_current_action(long long arg);
void initialize(int index);
void create_code_from_tree(struct Action* c);
int resize_action(struct Action* c);
void transform_tree_to_code();
void check_initilized(int var, int line, bool is_in_while);
void add_args_to_proc(int args);
bool is_argsnum_matched(int argscall, string procedure_name);
void add_start_program(struct Action* commands1, struct Action* commands2);
int get_args_number(int index);
void set_type_of_current_action(enum AbstractType type);
void transform_intermediate_code_to_asm();
int is_variable(int pos);
void div_or_mod(string operation, long long first, long long second, long long var_assign_to);
void add_asm_action(enum AssemblerActionType type, long long firstREG, long long secondREG, int dest);
void add_code_action(enum AbstractType type);
void write_asm_to_file(FILE* out);
void add_asm_action(enum AssemblerActionType type, long long firstREG, long long secondREG, int dest);
void translate();
void eq_or_neq(string what, long long first, long long second, long long jump);
bool add_or_sub_first_section(AssemblerActionType act, long long first, long long second, long long var_assign_to);
struct Action* add_procedure(struct Action* parent,struct Action* prochead, struct Action* child);
struct Action* add_empty(enum ActionType type);
struct Action* add_main(struct Action* Actions);
struct Action* add_action(struct Action* parent, struct Action* child);
struct Action* add_parent_action(enum ActionType type, int num_of_children, ...);
struct Action* add_proc_action(enum ActionType type, struct Action* child);
struct Action* add_proc_calling_action(enum ActionType type, struct Action* name, struct Action* arguments);
struct Action* add_add_arg_action(struct Action* parent, struct Action* child);
struct Action* add_empty_proc_action(enum ActionType type,struct Action* child);
struct Action* add_elseif_action(enum ActionType type, struct Action* exsp, struct Action* com, struct Action* ecom);
struct Action* add_if_action(enum ActionType type, struct Action* exsp, struct Action* com);
struct Action* add_while_action(enum ActionType type, struct Action* exspresions, struct Action* commands);
struct Action* add_repeat_action(enum ActionType type, struct Action* commands, struct Action* exspresions);
struct Action* add_call_action(enum ActionType type, struct Action* com);
struct Action* add_give_params_to_proc_action(int index, struct Action* arguments);
struct Action* add_give_params_to_proc(int h, int k);
struct Action* add_get_params_from_proc_action(int index, struct Action* arguments);
struct Action* add_get_params_from_proc(int h, int k);


struct AssemblerAction{
    enum AssemblerActionType type;
    long long firstREG;
    long long secondREG;
    int jumpPoint;           
};
 


struct Assembler{
    int size;
    int max;
    struct AssemblerAction* actions;
};

struct CodeAction{
    enum AbstractType type;
    int size;
    long long args[3];
};


struct IntermediateCode{
    int size;
    int max;
    struct CodeAction* actions;
};

struct Procedure{
    int index;
    int start;
    int end;
};

struct Element{
    enum ElementType type;   
    string name;
    bool is_initialized;
    int args;
};


struct Action{
    enum ActionType type;
    int index;  
    int size;
    int max;
    struct Action** actions;
   
};


