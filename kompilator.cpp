#include <stdio.h>
#include <string.h>

#include "mylib.hpp"
#include <iostream>
extern FILE* yyin;
int yyparse();
using namespace std;

int main(int argc,char* argv[]){  

    FILE* out;
    if(argc > 2){
        yyin = fopen(argv[1], "r");
        out = fopen(argv[2], "w");
    } else {
        cout << "Niepoprawna liczba argumentów: Użycie: ./kompilator <in> <out> "<<endl;
        return 1;
    }
    yyparse();

    fclose(yyin); 
    translate();
    write_asm_to_file(out);
    fclose(out);
    return 0;
}
