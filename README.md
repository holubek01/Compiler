# Kompilator prostego języka imperatywnego
Jest to program, który przekształca kod w języku imperatywnym na kod maszyny wirtualnej. Kompilator został napisany w języku C++ z wykorzystaniem narzędzi Flex oraz Bison. W pliku labor4.pdf znajdują się szczegółowe specyfikacje dotyczące języka i maszyny, a także przykładowy kod. W folderze *Compiler* znajduje się projekt kompilatora, natomiast w folderze *virtual-machine* znajduje się maszyna wirtualna, która czyta i wykonuje program, który został przetłumaczony przez kompilator.

## Sposób uruchomienia
Aby skompilować program należy użyć polecenia `make` w folderze *Compiler*. Następnie, aby przekształcić kod w języku imperatywnym na kod maszyny wirtualnej należy użyć polecenia `$./kompilator <plik_wejściowy> <plik_wyjściowy>` gdzie 
<plik_wejściowy> zawiera kod w języku imperatywnym, który zostanie przetłumaczony na kod maszyny do pliku <plik_wyjściowy>.
Następnie w celu odpalenia przekompilowanego kodu na maszynie wirtualnej należy użyć polecenia `./maszyna-wirtualna <plik wyjściowy>`.


## Narzędzia 
Program był kompilowany i testowany przy użyciu narzędzi:

- g++ 9.4.0
- flex 2.6.4
- bison (GNU Bison) 3.5.1
- GNU Make 4.2.1

## Autorzy
* [Tomasz Hołub](https://github.com/holubek01) - Kompilator
* [Dr Maciej Gębala](https://cs.pwr.edu.pl/gebala/) - Maszyna wirtualna

