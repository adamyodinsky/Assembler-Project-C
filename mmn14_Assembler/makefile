assembler: main.o cycle1And2.o data_list.o symbol_list.o word_list.o utils.o
	gcc -g main.o cycle1And2.o data_list.o symbol_list.o word_list.o utils.o -o assembler -lm

main.o: main.c cycle1And2.h MSG.h
	gcc -c -g -ansi -pedantic -Wall main.c -o main.o

cycle1And2.o: cycle1And2.c cycle1And2.h utils.h word_list.h symbol_list.h data_list.h MSG.h
	gcc -c -g -ansi -pedantic -Wall cycle1And2.c -o cycle1And2.o	-lm

utils.o: utils.c  utils.h MSG.h
	gcc -c -g -ansi -pedantic -Wall utils.c -o utils.o	

data_list.o: data_list.c  data_list.h 
	gcc -c -g -ansi -pedantic -Wall data_list.c -o data_list.o	

symbol_list.o: symbol_list.c  symbol_list.h MSG.h
	gcc -c -g -ansi -pedantic -Wall symbol_list.c -o symbol_list.o	

word_list.o: word_list.c  word_list.h MSG.h
	gcc -c -g -ansi -pedantic -Wall word_list.c -o word_list.o		