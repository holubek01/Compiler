#include <string.h>
#include "mylib.hpp"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <cstdio>
using namespace std;

struct Assembler* assemblerCommands;
struct IntermediateCode* intermediateCode; 
struct Action* tree;
int counter = 0;
vector<int> pairs;
int tmp = 0;
vector<Element> table_of_elements; 
vector<Procedure> table_of_procedures;


void create_element(ElementType type, string name, int args_num, bool default_init){
    struct Element element;
    element.type = type;
    element.name = name;
    element.args = args_num;
    element.is_initialized = default_init;
    table_of_elements.push_back(element);
}

void translate()
{
    init("intermediate");  
    transform_tree_to_code();            
    init("asm");
    transform_intermediate_code_to_asm(); 
}



int get_symbol_index(string name, int iter, string type){
    ElementType typ;
    if(type=="PROC") typ = PROC;
    else if(type == "VARIABLE") typ = VARIABLE;

    for(size_t i=iter; i<table_of_elements.size(); i++)
    {
    	if(!table_of_elements.at(i).name.compare(name) && table_of_elements.at(i).type==typ)
        {
            return i;
        }
    }
    return -1;
}

bool is_argsnum_matched(int argscall, string procedure_name)
{
    int counter = table_of_elements.size()-1;
    while(table_of_elements.at(counter).name!=procedure_name || table_of_elements.at(counter).type!=PROC)
    {
        counter--;
    }
    if(table_of_elements.at(counter).args != argscall) return false;
    return true;
}

void add_args_to_proc(int args){
    table_of_elements.at(table_of_elements.size()-1-args).args = args;
}



int is_declared_proc(string name, int iter){

    string proper_name = name.substr(0,name.length()-1);
    for(int i=0; i<iter; i++)
    {
    	if(table_of_elements.at(i).type==PROC && !table_of_elements.at(i).name.compare(proper_name))
        {
            return i;
        }
    }
    return -1;
}


void add_start_program(struct Action* actions1, struct Action* actions2)
{
    pairs.push_back(0);
    tree=(Action*)malloc(sizeof(struct Action));
	set_action_values2(tree, AST_PROGRAM_ALL, -1,4,4);
	tree->actions=(Action**)malloc(sizeof(struct Action*)*4);
    tree->actions[0]=add_action2(AST_PROCBLOCKSTART, pairs.size()-1);
	tree->actions[1]=actions1;
    tree->actions[2]=add_action2(AST_PROCBLOCKEND, pairs.size()-1);
	tree->actions[3]=actions2;
}

const char asmActionNames[][15] = {"GET", "PUT","LOAD", "STORE", "LOADI", "STOREI","ADD", "SUB", "ADDI", "SUBI", "SET","HALF", "JUMP", "JPOS","JZERO", 
"JUMPI", "HALT"};


int add_var_to_table(string name){
    create_element(VARIABLE, name, 0, false);
    return table_of_elements.size();
} 

void set_start_point(int* iter)
{
    *iter = (int) table_of_elements.size();
}

int add_proc_to_table(string name, int* iter){
    *iter=(int) table_of_elements.size();
    create_element(PROC, name, 0, false);
    return table_of_elements.size();
} 


int add_constant(string number){
    for(size_t i=0; i<table_of_elements.size(); i++){
        if(table_of_elements.at(i).type==CONSTANT){ 
            
            if(table_of_elements.at(i).name == number) {return i;}}
    }
    create_element(CONSTANT, number, 0, true);
    
    return table_of_elements.size()-1;
}


struct Action* add_proc_action(enum ActionType type, struct Action* child)
{
    struct Action* progra=(Action*)malloc(sizeof(struct Action));
    set_action_values2(progra, type, -1,1,1);
	progra->actions=(Action**)malloc(sizeof(struct Action*));
	progra->actions[0]=child;

    return progra;
}



void init(string what_to_init)
{
    if(what_to_init == "asm")
    {
        assemblerCommands=(struct Assembler*)malloc(sizeof(struct Assembler));
        assemblerCommands->size = 0;
        assemblerCommands->max=128;
        assemblerCommands->actions=(AssemblerAction*)malloc(sizeof(struct AssemblerAction)*128);
    }
    else if(what_to_init == "intermediate")
    {
        intermediateCode=(IntermediateCode*)malloc(sizeof(struct IntermediateCode));
        intermediateCode->size=0;
        intermediateCode->max=128;
        intermediateCode->actions=(CodeAction*)malloc(sizeof(struct CodeAction)*128);
    }
}



int resize_action(struct Action* ptr){
    void* realloc_helper=realloc(ptr->actions, sizeof(struct Action*)*ptr->max*2);
    if(!realloc_helper){
        cerr << "Błąd podczas realokowania pamięci"<<endl;
        return 1;
    }
    ptr->max*=2;
    ptr->actions=(Action**)realloc_helper;
    
    return 0;
}



int resize(string what_to_resize){
    void* realloc_helper;
    if (what_to_resize == "intermediateCode") realloc_helper=realloc(intermediateCode->actions, sizeof(struct CodeAction)*intermediateCode->max*2);
    if (what_to_resize == "assemblerCommands") realloc_helper= realloc(assemblerCommands->actions, sizeof(struct AssemblerAction)*assemblerCommands->max*2);
    
    if(!realloc_helper){
        cout<<"Błąd podczas realokowania pamięci"<<endl;
        return 1;
    }
    if (what_to_resize == "intermediateCode"){
        intermediateCode->max*=2;
        intermediateCode->actions=(CodeAction*)realloc_helper;
    }
    else if (what_to_resize == "assemblerCommands"){
        assemblerCommands->max*=2;
        assemblerCommands->actions=(AssemblerAction*)realloc_helper;
    }
    
    
    return 0;
}

struct Action* check_should_resize(Action* parent)
{
    if(parent->size == parent->max)
        if (resize_action(parent))
            return parent;
}


struct Action* add_procedure(struct Action* parent,struct Action* prochead, struct Action* child)
{
    struct Procedure* helpproc = (Procedure*)malloc(sizeof(struct Procedure));
    int i = table_of_procedures.size();
    table_of_procedures.push_back(*helpproc);
    check_should_resize(parent);
    
    parent->actions[parent->size]=prochead;
    parent->actions[parent->size]->index = i;
    parent->size++;
    check_should_resize(parent);
    parent->actions[parent->size]=child; 
    parent->size++;
    check_should_resize(parent);
    parent->actions[parent->size]=add_action2(AST_ENDPROCEDURE,i);
    parent->actions[parent->size]->index = i;
    parent->size++;
    return parent;
}



Action* set_action_values(ActionType type, int index, int size, int max)
{
    struct Action* ptr = (Action*)malloc(sizeof(struct Action));
    ptr->type=type;
    ptr->index=index;
    ptr->size=size;
    ptr->max=max;
    ptr->actions=NULL;
    return ptr;
}

void set_action_values2(Action* ptr, ActionType type, int index, int size, int max)
{
    ptr->type=type;
    ptr->index=index;
    ptr->size=size;
    ptr->max=max;
}


struct Action* add_empty(enum ActionType type)
{
	struct Action* ptr=(Action*)malloc(sizeof(struct Action));
    set_action_values2(ptr, type, -1, 0, 8);
    ptr->actions=(Action**)malloc(sizeof(struct Action*)*8);
    return ptr;
}


struct Action* add_main(struct Action* actions)
{
	struct Action* main=(Action*)malloc(sizeof(struct Action));
    set_action_values2(main, AST_MAIN, -1, 2, 1);
	main->actions=(Action**)malloc(sizeof(struct Action*));
	main->actions[0]=actions;
    main->actions[1]=add_action2(AST_ENDMAIN, -1);
	
	return main;
}


struct Action* add_action2(ActionType typ, int i)
{
    return set_action_values(typ, i, 0, 0);
}


struct Action* add_call_action(enum ActionType type, struct Action* com)
{
    struct Action* ptr=(Action*)malloc(sizeof(struct Action));
    set_action_values2(ptr, type, -1, 1, 1);
    ptr->actions=(Action**)malloc(sizeof(struct Action*));
    ptr->actions[0] = com;
    return ptr;
}

struct Action* add_proc_calling_action(enum ActionType type, struct Action* name, struct Action* arguments)
{
    struct Action* ptr=(Action*)malloc(sizeof(struct Action));
    set_action_values2(ptr, type, -1, 5, 5);
    ptr->actions=(Action**)malloc(sizeof(struct Action*)*5);
    ptr->actions[0] = name;
    ptr->actions[1] = arguments;
    ptr->actions[2] = add_give_params_to_proc_action(ptr->actions[0]->index+1, arguments);
    ptr->actions[4] = add_action2(AST_JUMPTOPROC,ptr->actions[0]->index+1);
    ptr->actions[3] = add_action2( AST_PROCRETURN,ptr->actions[0]->index+1);
    ptr->actions[5] = add_get_params_from_proc_action(ptr->actions[0]->index+1, arguments);
    return ptr;
}


struct Action* add_get_params_from_proc_action(int index, struct Action* arguments)
{
    struct Action* ptr=(Action*)malloc(sizeof(struct Action));
    set_action_values2(ptr, AST_GET_PARAMS_FROM_PROC, -1, arguments->size, arguments->size);
    ptr->actions=(Action**)malloc(sizeof(struct Action*)*arguments->size);
    for(int i = 0; i < arguments->size; i++)
    {
        ptr->actions[i] = add_get_params_from_proc(index + i , arguments->actions[i]->index);
    }
    return ptr;
}

struct Action* add_get_params_from_proc(int tmp, int k)
{
    struct Action* ptr=(Action*)malloc(sizeof(struct Action));
    set_action_values2(ptr, AST_GET_PARAMS, tmp, 1, 1);
    ptr->actions=(Action**)malloc(sizeof(struct Action*));
    ptr->actions[0] = add_action2(AST_IDENTIFIER, k);

    return ptr;
}

struct Action* add_give_params_to_proc_action(int index, struct Action* arguments)
{
    struct Action* ptr=(Action*)malloc(sizeof(struct Action));
    set_action_values2(ptr, AST_GIVE_PARAMS_TO_PROC, -1, arguments->size, arguments->size);
    ptr->actions=(Action**)malloc(sizeof(struct Action*)*arguments->size);
    for(int i = 0; i < arguments->size; i++)
    {
        ptr->actions[i] = add_give_params_to_proc(index + i , arguments->actions[i]->index);
    }
    return ptr;
}

struct Action* add_give_params_to_proc(int tmp, int k)
{
    struct Action* ptr=(Action*)malloc(sizeof(struct Action));
    set_action_values2(ptr, AST_GIVE_PARAMS, tmp, 1, 1);
    ptr->actions=(Action**)malloc(sizeof(struct Action*));
    ptr->actions[0] = add_action2(AST_IDENTIFIER, k);

    return ptr;
}

struct Action* add_if_action(enum ActionType type, struct Action* exsp, struct Action* com)
{
    pairs.push_back(0);
    struct Action* ptr=(Action*)malloc(sizeof(struct Action));
    set_action_values2(ptr, type, -1, 3, 3);
    ptr->actions=(Action**)malloc(sizeof(struct Action*) * 3);
    ptr->actions[0] = exsp;
    ptr->actions[1] = com;
    ptr->actions[0]->index = pairs.size()-1;
    ptr->actions[2] = add_action2(AST_ENDIF,pairs.size()-1);
    return ptr;
}



struct Action* add_action(struct Action* parent, struct Action* child)
{
	check_should_resize(parent);
    parent->actions[parent->size]=child; 
    parent->size++;
    return parent;
}


struct Action* add_elseif_action(enum ActionType type, struct Action* exsp, struct Action* com, struct Action* ecom)
{
    pairs.push_back(0);
    pairs.push_back(0);
    struct Action* ptr=(Action*)malloc(sizeof(struct Action));
    set_action_values2(ptr, type, -1, 6, 6);
    ptr->actions=(Action**)malloc(sizeof(struct Action*) * 6);
    ptr->actions[0] = exsp;
    ptr->actions[1] = com;
    ptr->actions[0]->index = pairs.size()-2;
    ptr->actions[2] = add_action2(AST_JUMPIF,pairs.size()-1);
    ptr->actions[3] = add_action2(AST_JUMPELSE,pairs.size()-2);
    ptr->actions[4] = ecom;
    ptr->actions[5] = add_action2(AST_ENDELSEIF,pairs.size()-1);
    return ptr;
}


struct Action* add_add_arg_action(struct Action* parent, struct Action* child)
{
    check_should_resize(parent);
    parent->actions[parent->size]=child; 
    parent->size++;
    
    return parent;
}

struct Action* add_empty_proc_action(enum ActionType type,struct Action* child)
{
    struct Action* ptr=(Action*)malloc(sizeof(struct Action));
    set_action_values2(ptr, type, -1, 0, 8);
    ptr->actions=(Action**)malloc(sizeof(struct Action*)*8);
    ptr->actions[ptr->size]=child; 
    ptr->size++;

    return ptr;
}



struct Action* add_while_action(enum ActionType type, struct Action* exspresions, struct Action* commands)
{
    pairs.push_back(0);
    pairs.push_back(0);
    struct Action* ptr=(Action*)malloc(sizeof(struct Action));
    set_action_values2(ptr, type, -1, 5, 5);
    ptr->actions=(Action**)malloc(sizeof(struct Action*) * 5);
    ptr->actions[0] = add_action2(AST_WHILEBEGIN,pairs.size()-1);
    ptr->actions[1] = exspresions;
    ptr->actions[1]->index = pairs.size()-2;
    ptr->actions[2] = commands;
    ptr->actions[3] = add_action2(AST_WHILEEND,pairs.size()-1);
    ptr->actions[4] = add_action2(AST_WHILEBREAK,pairs.size()-2);
    
    return ptr;
}



struct Action* add_repeat_action(enum ActionType type, struct Action* commands, struct Action* exspresions)
{
    pairs.push_back(0);
    pairs.push_back(0);

    struct Action* ptr=(Action*)malloc(sizeof(struct Action));
    set_action_values2(ptr, type, -1, 6, 6);

    ptr->actions=(Action**)malloc(sizeof(struct Action*) * 6);
    ptr->actions[0] = add_action2(AST_BEGINREPEAT, pairs.size()-2);
    ptr->actions[1] = commands;
    ptr->actions[2] = add_action2(AST_EMPTYREPEAT, -1);
    ptr->actions[3] = exspresions;
    ptr->actions[3]->index = pairs.size()-2;  
    ptr->actions[4] = add_action2(AST_JUMPREPEAT, pairs.size()-1);
    ptr->actions[5] = add_action2(AST_ENDREPEAT,pairs.size()-1);
    return ptr;
}


struct Action* add_parent_action(enum ActionType type, int num_of_children, ...)
{
    va_list ap;
    va_start(ap, num_of_children); 
    
    struct Action* ptr=(Action*)malloc(sizeof(struct Action));
    set_action_values2(ptr, type, -1, num_of_children, num_of_children);
    ptr->actions=(Action**)malloc(sizeof(struct Action*) * num_of_children);
 
    for(int i=0; i<num_of_children; i++){
        ptr->actions[i]=va_arg(ap, struct Action*);
    }
    
    va_end(ap);
    
    return ptr;
}


void add_code_action(enum AbstractType type){
    if(intermediateCode->size == intermediateCode->max) resize("intermediateCode");
        
    intermediateCode->actions[intermediateCode->size].type=type;
    intermediateCode->actions[intermediateCode->size].size=0;
    intermediateCode->size++;
}


void set_args_for_action(long long arg){
    intermediateCode->actions[intermediateCode->size-1].args[intermediateCode->actions[intermediateCode->size-1].size]=arg;
    intermediateCode->actions[intermediateCode->size-1].size++;
}


void create_code_from_tree(struct Action* ptr){
    if(!ptr) return;
    AbstractType helper;
    ActionType typ = ptr->type;

    if(typ == AST_ASSIGN)
    {
        add_code_action(ABSTRACT_OVERWRITE);
        create_code_from_tree(ptr->actions[0]);
        create_code_from_tree(ptr->actions[1]);
    }
    
    else if (typ == AST_NUM ||typ == AST_IDENTIFIER)
    {
        set_args_for_action(ptr->index);
    }

    else if (typ == AST_ADD ||typ == AST_SUB || typ == AST_MUL ||typ == AST_DIV || typ == AST_MOD)
    {
        intermediateCode->actions[intermediateCode->size-1].type= AbstractType(ptr->type);
        create_code_from_tree(ptr->actions[0]);
        create_code_from_tree(ptr->actions[1]);
    } 
    else if (typ == AST_ARGS)
    {
        add_code_action(ABSTRACT_ARGS);
        for(int i=0; i<ptr->size; i++){  
            
            create_code_from_tree(ptr->actions[i]);
        }
    }  

    else if (typ == AST_PROGRAM_ALL ||typ == AST_MAIN || typ == AST_PROCEDURE ||typ == AST_COMMANDS)
    {
        for(int i=0; i<ptr->size; i++){  
            create_code_from_tree(ptr->actions[i]);
        }
    } 

    else if (typ == AST_IF)
    {
        add_code_action(ABSTRACT_IF);
        create_code_from_tree(ptr->actions[0]);
        create_code_from_tree(ptr->actions[1]);
        create_code_from_tree(ptr->actions[2]);
    } 
    else if (typ == AST_IFELSE || typ == AST_REPEAT || typ == AST_PROCCALLING)
    {
        helper = AbstractType(ptr->type);
        add_code_action(helper);
        create_code_from_tree(ptr->actions[0]);
        create_code_from_tree(ptr->actions[1]);
        create_code_from_tree(ptr->actions[2]);
        create_code_from_tree(ptr->actions[3]);
        create_code_from_tree(ptr->actions[4]);
        create_code_from_tree(ptr->actions[5]);
    } 

    else if (typ == AST_LT ||typ == AST_GT || typ == AST_LEQ ||typ == AST_GEQ || typ == AST_NEQ || typ == AST_EQ)
    {
        intermediateCode->actions[intermediateCode->size-1].type= AbstractType(ptr->type);
        create_code_from_tree(ptr->actions[0]);
        create_code_from_tree(ptr->actions[1]);
        set_args_for_action(ptr->index);
    } 

    else if (typ == AST_WHILE)
    {
        add_code_action(ABSTRACT_WHILE);
        create_code_from_tree(ptr->actions[0]);
        create_code_from_tree(ptr->actions[1]);
        create_code_from_tree(ptr->actions[2]);
        create_code_from_tree(ptr->actions[3]);
        create_code_from_tree(ptr->actions[4]);
    } 


    else if (typ == AST_ENDMAIN)
    {
        add_code_action(ABSTRACT_HALT);
    } 

    else if (typ == AST_EMPTYREPEAT)
    {
        add_code_action(ABSTRACT_BEGINREPEAT);
    } 

    else if (typ == AST_READ ||typ == AST_WRITE)
    {
        helper = AbstractType(ptr->type);
        add_code_action(helper);
        create_code_from_tree(ptr->actions[0]);
    } 

    else if (typ == AST_WHILEBEGIN)
    {
        add_code_action(ABSTRACT_WHILEBEGIN);
        set_args_for_action(ptr->index);
        add_code_action(ABSTRACT_WHILEBEGIN);
    } 

    
    else if (typ == AST_JUMPTOPROC ||typ == AST_PROCRETURN || typ == AST_ENDPROCEDURE ||typ == AST_WHILEBREAK ||
         typ == AST_WHILEEND || typ == AST_ENDELSEIF || typ == AST_JUMPELSE ||typ == AST_ENDIF || 
         typ == AST_PROCBLOCKSTART ||typ == AST_JUMPIF ||
         typ == AST_BEGINREPEAT ||typ == AST_ENDREPEAT || typ == AST_JUMPREPEAT ||typ == AST_PROCBLOCKEND  )
    {
        helper = AbstractType(ptr->type);
        add_code_action(helper);
        set_args_for_action(ptr->index);
    }

    else if (typ == AST_GIVE_PARAMS_TO_PROC || typ == AST_GET_PARAMS_FROM_PROC)
    {
        helper = AbstractType(ptr->type);
        add_code_action(helper);
        for(int i=0; i<ptr->size; i++){  
            
            create_code_from_tree(ptr->actions[i]);
        }
    } 

    else if (typ == AST_GET_PARAMS ||typ == AST_GIVE_PARAMS || typ == AST_CALL ||typ == AST_ADD_PROCEDURE)
    {
        helper = AbstractType(ptr->type);
        add_code_action(helper);
        create_code_from_tree(ptr->actions[0]);
        set_args_for_action(ptr->index);
    }
}

void transform_tree_to_code(){   
    create_code_from_tree(tree);
}


void add_action(enum AssemblerActionType type, long long firstREG, long long secondREG, int dest){
    if(assemblerCommands->size == assemblerCommands->max) resize("assemblerCommands");

    int size = assemblerCommands->size;

    assemblerCommands->actions[size].type=type;
    assemblerCommands->actions[size].firstREG=firstREG;
    assemblerCommands->actions[size].secondREG=secondREG;
    assemblerCommands->actions[size].jumpPoint=dest;
    counter++;
    assemblerCommands->size++;
}



int is_variable(int index) {
    if(table_of_elements.at(index).type == VARIABLE) return 1;     
    return 0;         
}

int get_args_number(int index) { return table_of_elements.at(index).args;}


void check_initilized(int var, int line, bool is_in_while)
{
    if(table_of_elements.at(var).is_initialized == false)
    {
        if(is_in_while)
        {
            cout<<endl;
            cout<<"WARNING: Możliwe użycie niezainicjalizowanej zmiennej " << table_of_elements.at(var).name << " w linii "<< line <<endl;
        }
        else{
            cout<<endl;
            cerr<<"Błąd: Użycie nie zainicjalizowanej zmiennej " << table_of_elements.at(var).name << " w linii "<< line <<endl;
            exit(0);
        }
        
    }
}


void initialize(int index)
{
    table_of_elements.at(index).is_initialized = true;
}




void transform_intermediate_code_to_asm(){
    for(int i=0; i<intermediateCode->size; i++){
        
        long long var_assign_to = intermediateCode->actions[i].args[0];
        long long first = intermediateCode->actions[i].args[1];
        long long second = intermediateCode->actions[i].args[2];
        
        int constant;
        int var;
        int jump;

        AbstractType typ = intermediateCode->actions[i].type;

        if(typ == ABSTRACT_OVERWRITE)
        {
            if(is_variable(first)) {
                add_action(LOAD_ASSEMLBER,first + 1,-1,-1);
                add_action(STORE_ASSEMLBER,var_assign_to + 1,-1,-1);    
            } else {
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(first).name),0, -1);               
                add_action(STORE_ASSEMLBER, var_assign_to + 1,-1,-1); 
                
            }
        }
        else if (typ == ABSTRACT_ADD)
        {
            //pierwsza część jest taka sama więc została przeniesiona do osobnej funkcji
            if(add_or_sub_first_section(ADD_ASSEMLBER, first, second, var_assign_to)){
                //trzeba się dowiedzieć która to jest stała
                if(!is_variable(first)) {constant = first; var = second;}
                else {constant = second; var = first;}
                
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(constant).name),0, -1);
                add_action(ADD_ASSEMLBER, (var + 1),-1,-1);
            	add_action(STORE_ASSEMLBER,var_assign_to + 1,-1,-1); 
            } 
        }
        else if (typ == ABSTRACT_SUB)
        {
            if(add_or_sub_first_section(SUB_ASSEMLBER, first, second, var_assign_to))
            {
                if(is_variable(first) && !is_variable(second)){
                
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(second).name),0, -1);
                add_action(STORE_ASSEMLBER,second + 1,-1,-1); 
                add_action(LOAD_ASSEMLBER,first + 1,-1,-1);
                add_action(SUB_ASSEMLBER, second + 1,-1,-1);
            	add_action(STORE_ASSEMLBER,var_assign_to + 1,-1,-1); 
                
            } else if (!is_variable(first) && is_variable(second)){
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(first).name),0, -1);
                add_action(SUB_ASSEMLBER, second + 1,-1,-1);
            	add_action(STORE_ASSEMLBER,var_assign_to + 1,-1,-1); 
                
            } 
            }
        }

        else if (typ == ABSTRACT_MUL)
        {
            int size = table_of_elements.size();
            add_action(SET_ASSEMLBER,0,-1,-1);
            add_action(STORE_ASSEMLBER,size + 1,-1,-1);
            
            if (!is_variable(first) && !is_variable(second)) {
            
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(second).name),-1,-1);
                add_action(STORE_ASSEMLBER,second + 1,-1,-1); 
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(first).name),-1,-1);
                add_action(STORE_ASSEMLBER,first+ 1,-1,-1); 
            }

            else if ((!is_variable(first) || !is_variable(second))){
                if(!is_variable(first)) {constant = first;}
                else {constant = second;}

                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(constant).name),-1,-1);
                add_action(STORE_ASSEMLBER,constant + 1,-1,-1); 

            }

            add_action(LOAD_ASSEMLBER,first + 1,-1,-1);
            add_action(SUB_ASSEMLBER,second + 1,-1,-1);
            add_action(JPOS_ASSEMLBER, -1,-1,counter + 6);
            add_action(LOAD_ASSEMLBER, first + 1,-1,-1);
            add_action(STORE_ASSEMLBER,size + 2,-1,-1);
            add_action(LOAD_ASSEMLBER, second + 1,-1,-1);
            add_action(STORE_ASSEMLBER,size + 3,-1,-1);
            add_action(JUMP_ASSEMLBER, -1,-1,counter + 5);
            add_action(LOAD_ASSEMLBER, first + 1,-1,-1);
            add_action(STORE_ASSEMLBER,size + 3,-1,-1);
            add_action(LOAD_ASSEMLBER, second + 1,-1,-1);
            add_action(STORE_ASSEMLBER,size + 2,-1,-1);
            add_action(LOAD_ASSEMLBER, size + 2,-1,-1);
            add_action(JZERO_ASSEMLBER, -1,-1,counter + 18);
            add_action(LOAD_ASSEMLBER, size + 2,-1,-1);
            add_action(HALF_ASSEMLBER, -1,-1,-1);
            add_action(ADD_ASSEMLBER, 0,-1,-1);
            add_action(STORE_ASSEMLBER,size + 4,-1,-1);
            add_action(LOAD_ASSEMLBER, size + 2,-1,-1);
            add_action(SUB_ASSEMLBER,size + 4,-1,-1);
            add_action(JZERO_ASSEMLBER, -1,-1,counter + 4);
            add_action(LOAD_ASSEMLBER, size + 1,-1,-1);
            add_action(ADD_ASSEMLBER, size + 3,-1,-1);
            add_action(STORE_ASSEMLBER,size + 1,-1,-1);
            add_action(LOAD_ASSEMLBER, size + 3,-1,-1);
            add_action(ADD_ASSEMLBER, 0,-1,-1);
            add_action(STORE_ASSEMLBER,size + 3,-1,-1);
            add_action(LOAD_ASSEMLBER, size + 2,-1,-1);
            add_action(HALF_ASSEMLBER, -1,-1,-1);
            add_action(STORE_ASSEMLBER,size + 2,-1,-1);
            add_action(JUMP_ASSEMLBER, -1,-1,counter - 18);
            add_action(LOAD_ASSEMLBER, size + 1,-1,-1);
            add_action(STORE_ASSEMLBER,var_assign_to + 1,-1,-1);
        }

        else if (typ == ABSTRACT_DIV)
        {
            div_or_mod("DIV", first, second, var_assign_to);
        }

        else if (typ == ABSTRACT_MOD)
        {
            div_or_mod("MOD", first, second, var_assign_to);
        }

        else if (typ == ABSTRACT_WRITE)
        {
            first = var_assign_to;

            if(!is_variable(first)){
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(first).name),0, -1);
                add_action(PUT_ASSEMLBER,0,-1,-1);
            } else {
                add_action(PUT_ASSEMLBER,first + 1,-1,-1);
            }

        }
       
        else if (typ == ABSTRACT_PROCBLOCKSTART)
        {
            add_action(SET_ASSEMLBER, 0,-1,-1);
            add_action(JZERO_ASSEMLBER,-1,var_assign_to,-1); 

        }

        else if (typ == ABSTRACT_ADD_PROCEDURE)
        {
            table_of_procedures.at(first).index = var_assign_to;  
            table_of_procedures.at(first).start = counter;
        }
    
        else if (typ == ABSTRACT_ENDPROCEDURE)
        {
            add_action(LOAD_ASSEMLBER,table_of_elements.size() + var_assign_to + 8,-1,-1);
            add_action(JUMPI_ASSEMLBER, -1,-1,0);
        }


        else if (typ == ABSTRACT_GIVE_PARAMS)
        {
            add_action(LOAD_ASSEMLBER,var_assign_to + 1,-1,-1);
            add_action(STORE_ASSEMLBER,first + 1,-1,-1); 
        }

        else if (typ == ABSTRACT_GET_PARAMS)
        {
            add_action(LOAD_ASSEMLBER,first + 1,-1,-1);
            add_action(STORE_ASSEMLBER,var_assign_to + 1,-1,-1); 
        }

        else if (typ == ABSTRACT_JUMPTOPROC)
        {
           for(int j = 0; j < (int) table_of_procedures.size(); j++)
            {
                if(var_assign_to  == table_of_procedures.at(j).index)
                {
                    tmp = j;
                }
            }
            
            add_action(JUMP_ASSEMLBER, -1,-1,table_of_procedures.at(tmp).start);
        }

        else if (typ == ABSTRACT_PROCRETURN)
        {
           for(int j = 0; j < (int) table_of_procedures.size(); j++)
            {
                if(var_assign_to  == table_of_procedures.at(j).index)
                {
                    tmp = j;
                }
            }
            add_action(SET_ASSEMLBER,counter+3,-1,-1); 
            add_action(STORE_ASSEMLBER,table_of_elements.size() + tmp + 8,-1,-1); 
            table_of_procedures.at(tmp).end = counter;
        }

        else if (typ == ABSTRACT_READ)
        {
            first = var_assign_to;
            add_action(GET_ASSEMLBER,first + 1,-1,-1);
        }

        else if (typ == ABSTRACT_HALT)
        {
            first = var_assign_to;
            add_action(HALT_ASSEMLBER,-1,-1,-1);
        }

        else if (typ == ABSTRACT_ENDIF|| typ == ABSTRACT_JUMPELSE || typ == ABSTRACT_ENDELSEIF || typ == ABSTRACT_BEGINREPEAT ||
                typ == ABSTRACT_WHILEBEGIN || typ == ABSTRACT_ENDREPEAT || typ == ABSTRACT_WHILEBREAK || typ == ABSTRACT_PROCBLOCKEND) 
        {
            first = var_assign_to;
            pairs.at(first) = counter;
        }


        else if (typ == ABSTRACT_JUMPREPEAT|| typ == ABSTRACT_WHILEEND || typ == ABSTRACT_JUMPIF) 
        {
            first = var_assign_to;
            add_action(SET_ASSEMLBER, 0,-1,-1);
            add_action(JZERO_ASSEMLBER,-1,first,-1); 
        }

        else if (typ == ABSTRACT_NEQ)
        {
            jump = second;
            second = first;
            first = var_assign_to;

            eq_or_neq("NEQ", first, second, jump);
            add_action(JZERO_ASSEMLBER, -1, jump, -1);
        }

        else if (typ == ABSTRACT_EQ)
        {
            //EQ I NEQ to to samo tylko inny jump na końcu
            jump = second;
            second = first;
            first = var_assign_to;

            eq_or_neq("EQ",first, second, jump);
            add_action(JPOS_ASSEMLBER, -1, jump, -1);
        }

        else if (typ == ABSTRACT_LT)
        {
            jump = second;
            second = first;
            first = var_assign_to;
            int size = table_of_elements.size();
       
            add_action(SET_ASSEMLBER, 1,0, -1);
            add_action(STORE_ASSEMLBER,size + 1,-1,-1); 
            
            if(is_variable(first) && is_variable(second)) {
                add_action(LOAD_ASSEMLBER,first + 1,-1,-1);
                add_action(ADD_ASSEMLBER, size + 1,-1,-1);
                add_action(SUB_ASSEMLBER, second + 1,-1,-1);

            }else if (!is_variable(first) && !is_variable(second)) {
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(second).name),0, -1);
            	add_action(STORE_ASSEMLBER,second + 1,-1,-1); 
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(first).name),0, -1);
                add_action(ADD_ASSEMLBER, size + 1,-1,-1);
                add_action(SUB_ASSEMLBER, second + 1,-1,-1);

            } else {
                if(!is_variable(first)) {constant = first; var = second;}
                else {constant = second; var = first;}

                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(constant).name),0, -1);
                add_action(STORE_ASSEMLBER,constant + 1,-1,-1); 
                add_action(LOAD_ASSEMLBER, first +1 ,-1,-1);
                add_action(ADD_ASSEMLBER, size + 1,-1,-1);
                add_action(SUB_ASSEMLBER, second + 1,-1,-1);
            }

            add_action(JPOS_ASSEMLBER, -1, jump, -1);
        }

        else if (typ == ABSTRACT_GT)
        {
            jump = second;
            second = first;
            first = var_assign_to;
            int size = table_of_elements.size();

            add_action(SET_ASSEMLBER, 1,0, -1);
            add_action(STORE_ASSEMLBER,size + 1,-1,-1); 

            if(is_variable(first) && is_variable(second)) {
                add_action(LOAD_ASSEMLBER,second + 1,-1,-1);
                add_action(ADD_ASSEMLBER, size + 1,-1,-1);
                add_action(SUB_ASSEMLBER, first + 1,-1,-1);

            } 
            else if(!is_variable(first) && !is_variable(second))
            {
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(first).name),0, -1);
            	add_action(STORE_ASSEMLBER,first + 1,-1,-1); 
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(second).name),0, -1);
                add_action(ADD_ASSEMLBER, size + 1,-1,-1);
                add_action(SUB_ASSEMLBER, first + 1,-1,-1);
            }
            else{
                if(!is_variable(first)) {constant = first; var = second;}
                else {constant = second; var = first;}

                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(constant).name),0, -1);
                add_action(STORE_ASSEMLBER,constant + 1,-1,-1); 
                add_action(LOAD_ASSEMLBER, second +1 ,-1,-1);
                add_action(ADD_ASSEMLBER, size + 1,-1,-1);
                add_action(SUB_ASSEMLBER, first + 1,-1,-1);
            }
            
            add_action(JPOS_ASSEMLBER, -1, jump, -1);
        }


        else if (typ == ABSTRACT_LEQ)
        {
            jump = second;
            second = first;
            first = var_assign_to;

            if(is_variable(first) && is_variable(second)) {
                add_action(LOAD_ASSEMLBER,first + 1,-1,-1);
                add_action(SUB_ASSEMLBER, second + 1,-1,-1);
                
            } 
            else if (!is_variable(first) && !is_variable(second))
            {
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(second).name),0, -1);
            	add_action(STORE_ASSEMLBER,second + 1,-1,-1); 
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(first).name),0, -1);
                add_action(SUB_ASSEMLBER, second + 1,-1,-1);
            }
            else{
                if(!is_variable(first)) {constant = first; var = second;}
                else {constant = second; var = first;}

                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(constant).name),0, -1);
                add_action(STORE_ASSEMLBER,constant + 1,-1,-1); 
                add_action(LOAD_ASSEMLBER,first + 1,-1,-1);
                add_action(SUB_ASSEMLBER, second + 1,-1,-1);
            }

            add_action(JPOS_ASSEMLBER, -1, jump, -1);
        }

        else if (typ == ABSTRACT_GEQ)
        {
            jump = second;
            second = first;
            first = var_assign_to;

            if(is_variable(first) && is_variable(second)) {
                add_action(LOAD_ASSEMLBER,second + 1,-1,-1);
                add_action(SUB_ASSEMLBER, first + 1,-1,-1);
                
            } 
            else if(!is_variable(first) && !is_variable(second)){
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(first).name),0, -1);
            	add_action(STORE_ASSEMLBER,first + 1,-1,-1); 
                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(second).name),0, -1);
                add_action(SUB_ASSEMLBER, first + 1,-1,-1);
            }
            else{
                if(!is_variable(first)) {constant = first; var = second;}
                else {constant = second; var = first;}

                add_action(SET_ASSEMLBER, stoll(table_of_elements.at(constant).name),0, -1);
                add_action(STORE_ASSEMLBER,constant + 1,-1,-1); 
                add_action(LOAD_ASSEMLBER,second + 1,-1,-1);
                add_action(SUB_ASSEMLBER, first + 1,-1,-1);
            }
            
            add_action(JPOS_ASSEMLBER, -1, jump, -1);
        } 
    }
}



bool add_or_sub_first_section(AssemblerActionType act, long long first, long long second, long long var_assign_to)
{
    if(is_variable(first) && is_variable(second)){
        add_action(LOAD_ASSEMLBER,first + 1,-1,-1);
        add_action(act, second + 1,-1,-1);
        add_action(STORE_ASSEMLBER,var_assign_to + 1,-1,-1); 
        return false;
        
    } 

    else if (!is_variable(first) && !is_variable(second)) {
        
        add_action(SET_ASSEMLBER, stoll(table_of_elements.at(second).name),0, -1);
        add_action(STORE_ASSEMLBER,second + 1,-1,-1); 
        add_action(SET_ASSEMLBER, stoll(table_of_elements.at(first).name),0, -1);
        add_action(act, second + 1,-1,-1);
        add_action(STORE_ASSEMLBER,var_assign_to + 1,-1,-1); 
        return false;
    }
    return true;
}



void eq_or_neq(string what, long long first, long long second, long long jump)
{
    int constant;
    int var;

    if(is_variable(first) && is_variable(second)) {
        add_action(LOAD_ASSEMLBER,first + 1,-1,-1);
        add_action(SUB_ASSEMLBER, second + 1,-1,-1);
    } 
    //obie stałe
    else if (!is_variable(first) && !is_variable(second)) {
        add_action(SET_ASSEMLBER, stoll(table_of_elements.at(second).name),0, -1);
        add_action(STORE_ASSEMLBER,second + 1,-1,-1); 
        add_action(SET_ASSEMLBER, stoll(table_of_elements.at(first).name),0, -1);
        add_action(STORE_ASSEMLBER,first + 1, -1,-1);
        add_action(SUB_ASSEMLBER, second + 1,-1,-1);
    }

    //jesli 1 z nich stała
    else {
        if(!is_variable(first)) {constant = first; var = second;}
        else {constant = second; var = first;}

        add_action(SET_ASSEMLBER, stoll(table_of_elements.at(constant).name),0, -1);
        add_action(STORE_ASSEMLBER,constant + 1,-1,-1); 
        add_action(LOAD_ASSEMLBER, first +1 ,-1,-1);
        add_action(SUB_ASSEMLBER, second + 1,-1,-1);
    } 

    if(what == "NEQ") add_action(JPOS_ASSEMLBER, -1, -1, counter+4);
    else if(what == "EQ") add_action(JPOS_ASSEMLBER, -1, -1, counter+3);

    add_action(LOAD_ASSEMLBER,second + 1,-1,-1);
    add_action(SUB_ASSEMLBER, first + 1,-1,-1);
}


void div_or_mod(string operation, long long first,long long second, long long var_assign_to)
{
    int constant;
    int size = table_of_elements.size();

    //obie stałe
    if (!is_variable(first) && !is_variable(second)) {
        add_action(SET_ASSEMLBER, stoll(table_of_elements.at(first).name),-1,-1);
        add_action(STORE_ASSEMLBER,first + 1,-1,-1); 
        add_action(SET_ASSEMLBER, stoll(table_of_elements.at(second).name),-1,-1);
        add_action(STORE_ASSEMLBER,second + 1,-1,-1); 
    }

    //1 stała 
    else if ((!is_variable(first) || !is_variable(second))){
        if(!is_variable(first)) {constant = first;}
        else {constant = second;}

        add_action(SET_ASSEMLBER, stoll(table_of_elements.at(constant).name),-1,-1);
        add_action(STORE_ASSEMLBER,constant + 1,-1,-1); 
    }

    add_action(SET_ASSEMLBER,0,-1,-1);
    add_action(STORE_ASSEMLBER,size + 3,-1,-1);
    add_action(LOAD_ASSEMLBER, first + 1,-1,-1);
    add_action(STORE_ASSEMLBER,size + 1,-1,-1);
    add_action(LOAD_ASSEMLBER, second + 1,-1,-1);
    add_action(JPOS_ASSEMLBER, -1,-1,counter + 3);
    add_action(STORE_ASSEMLBER,size + 1,-1,-1);
    add_action(JUMP_ASSEMLBER, -1,-1,counter + 30);
    add_action(SUB_ASSEMLBER,size + 1,-1,-1);
    add_action(JPOS_ASSEMLBER, -1,-1,counter + 28);
    add_action(LOAD_ASSEMLBER, second + 1,-1,-1);
    add_action(STORE_ASSEMLBER,size + 2,-1,-1);
    add_action(SET_ASSEMLBER,1,-1,-1);
    add_action(STORE_ASSEMLBER,size + 4,-1,-1);
    add_action(LOAD_ASSEMLBER, size + 2,-1,-1);
    add_action(SUB_ASSEMLBER, size + 1,-1,-1);
    add_action(JPOS_ASSEMLBER, -1,-1,counter + 8);
    add_action(LOAD_ASSEMLBER, size + 2,-1,-1);
    add_action(ADD_ASSEMLBER,0,-1,-1);
    add_action(STORE_ASSEMLBER, size + 2,-1,-1);
    add_action(LOAD_ASSEMLBER, size + 4,-1,-1);
    add_action(ADD_ASSEMLBER,0,-1,-1);
    add_action(STORE_ASSEMLBER, size + 4,-1,-1);
    add_action(JUMP_ASSEMLBER, -1,-1,counter - 9);
    add_action(LOAD_ASSEMLBER, size + 2,-1,-1);
    add_action(HALF_ASSEMLBER, -1,-1,-1);
    add_action(STORE_ASSEMLBER, size + 2,-1,-1);
    add_action(LOAD_ASSEMLBER, size + 4,-1,-1);
    add_action(HALF_ASSEMLBER, -1,-1,-1);
    add_action(STORE_ASSEMLBER, size + 4,-1,-1);
    add_action(LOAD_ASSEMLBER, size + 3,-1,-1);
    add_action(ADD_ASSEMLBER,size + 4,-1,-1);
    add_action(STORE_ASSEMLBER, size + 3,-1,-1);
    add_action(LOAD_ASSEMLBER, size + 1,-1,-1);
    add_action(SUB_ASSEMLBER,size + 2,-1,-1);
    add_action(STORE_ASSEMLBER, size + 1,-1,-1);
    add_action(JUMP_ASSEMLBER, -1,-1,counter - 32);

    if(operation == "MOD") add_action(LOAD_ASSEMLBER, size + 1,-1,-1);             //zwracamy resztę
    else if(operation == "DIV") add_action(LOAD_ASSEMLBER, size + 3,-1,-1);        //zwracamy wynik

    add_action(STORE_ASSEMLBER, var_assign_to + 1,-1,-1);
}




void write_asm_to_file(FILE* out){
    cout<<endl<<"Kompilacja przebiegła pomyślnie"<<endl;

    for(int i=0; i<assemblerCommands->size; i++){
        if(assemblerCommands->actions[i].type == ADD_ASSEMLBER){
            fprintf(out, "%s %lld\n",  asmActionNames[assemblerCommands->actions[i].type], assemblerCommands->actions[i].firstREG);
        } else if(assemblerCommands->actions[i].type == SUB_ASSEMLBER){
            fprintf(out, "%s %lld\n",  asmActionNames[assemblerCommands->actions[i].type], assemblerCommands->actions[i].firstREG);
        } else if(assemblerCommands->actions[i].type == SET_ASSEMLBER){
            fprintf(out, "%s %llu\n",  asmActionNames[assemblerCommands->actions[i].type], assemblerCommands->actions[i].firstREG);
        } else if(assemblerCommands->actions[i].type == STORE_ASSEMLBER){
            fprintf(out, "%s %lld\n",  asmActionNames[assemblerCommands->actions[i].type], assemblerCommands->actions[i].firstREG);
        } else if(assemblerCommands->actions[i].type == LOAD_ASSEMLBER){
            fprintf(out, "%s %lld\n",  asmActionNames[assemblerCommands->actions[i].type], assemblerCommands->actions[i].firstREG);
        } else if(assemblerCommands->actions[i].type == PUT_ASSEMLBER){
            fprintf(out, "%s %lld\n",  asmActionNames[assemblerCommands->actions[i].type], assemblerCommands->actions[i].firstREG);
        } else if(assemblerCommands->actions[i].type == GET_ASSEMLBER){
            fprintf(out, "%s %lld\n",  asmActionNames[assemblerCommands->actions[i].type], assemblerCommands->actions[i].firstREG);
        } else if(assemblerCommands->actions[i].type == HALT_ASSEMLBER){
            fprintf(out, "%s \n", asmActionNames[assemblerCommands->actions[i].type]);
        } else if(assemblerCommands->actions[i].type == JPOS_ASSEMLBER){
            if(assemblerCommands->actions[i].jumpPoint == -1){
                assemblerCommands->actions[i].jumpPoint = pairs.at(assemblerCommands->actions[i].secondREG);
            }
            fprintf(out, "%s %d\n", asmActionNames[assemblerCommands->actions[i].type], assemblerCommands->actions[i].jumpPoint);
        } else if(assemblerCommands->actions[i].type == JUMP_ASSEMLBER){
            fprintf(out, "%s %d\n", asmActionNames[assemblerCommands->actions[i].type], assemblerCommands->actions[i].jumpPoint);
        } else if(assemblerCommands->actions[i].type == JZERO_ASSEMLBER){
            if(assemblerCommands->actions[i].jumpPoint == -1){
                assemblerCommands->actions[i].jumpPoint = pairs.at(assemblerCommands->actions[i].secondREG);
            }
            fprintf(out, "%s %d\n", asmActionNames[assemblerCommands->actions[i].type], assemblerCommands->actions[i].jumpPoint);
        } else if(assemblerCommands->actions[i].type == HALF_ASSEMLBER){
            fprintf(out, "%s \n", asmActionNames[assemblerCommands->actions[i].type]);
        } else if(assemblerCommands->actions[i].type == JUMPI_ASSEMLBER){
            fprintf(out, "%s %d\n", asmActionNames[assemblerCommands->actions[i].type], assemblerCommands->actions[i].jumpPoint);
        } 
    }
}

