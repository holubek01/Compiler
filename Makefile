CC = g++
FLAGS = -W -Wall -pedantic -std=c++17 -O3
FILES = parser.o lexer.o kompilator.o mylib.o

all: kompilator clean

kompilator: $(FILES)
	$(CC) -o kompilator $(FILES)

%.o: %.cpp
	$(CC) $(FLAGS) -c $^

parser.cpp: parser.y
	bison -d -o parser.cpp parser.y
	
lexer.cpp: lexer.l
	flex -o lexer.cpp lexer.l
	
mylib.o: mylib.hpp mylib.cpp
	g++ -c ${FLAGS} -o mylib.o mylib.cpp
	

clean:
	rm -f *.o lexer.cpp parser.hpp parser.cpp
