/*
* Kod kompilatora do projektu z JFTT 2022-2023
* Autor: Tomasz Hołub
* Nr indeksu: 261718
*/

-----------------------------------------------------------
Aby skompilować program należy użyć polecenia:
	$ make
-----------------------------------------------------------


Użycie kompilatora:
	$./kompilator <plik_wejściowy> <plik_wyjściowy>
-----------------------------------------------------------


Program był kompilowany i testowany przy użyciu narzędzi:

g++ 9.4.0
flex 2.6.4
bison (GNU Bison) 3.5.1
GNU Make 4.2.1
-----------------------------------------------------------


Pliki:

Makefile - automatyczna kompilacja
lexer.l - Lexer(Flex)
parser.y - Parser(Bison)
kompilator.cpp - Funkcja main programu
mylib.cpp - Główna biblioteka programu
mylib.hpp - Plik nagłówkowy głównej biblioteki programu
