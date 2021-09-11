assembler: assembler.o file_handler.o create_output.o error_handler.o \
           data_structure.o stage_one_func.o stage_one.o stage_two.o conversions.o
	gcc -ansi -pedantic -Wall -o assembler assembler.o \
	file_handler.o error_handler.o create_output.o data_structure.o \
	conversions.o stage_one.o stage_one_func.o stage_two.o 
	
assembler.o: assembler.c assembler.h
	gcc -ansi -pedantic -Wall -c assembler.c

file_handler.o: file_handler.c file_handler.h
	gcc -ansi -pedantic -Wall -c file_handler.c

create_output.o: create_output.c create_output.h
	gcc -ansi -pedantic -Wall -c create_output.c

error_handler.o: error_handler.c error_handler.h
	gcc -ansi -pedantic -Wall -c error_handler.c

data_structure.o: data_structure.c data_structure.h
	gcc -ansi -pedantic -Wall -c data_structure.c

stage_one.o: stage_one.c stage_one.h 
	gcc -ansi -pedantic -Wall -c stage_one.c

stage_one_func.o: stage_one_func.c stage_one_func.h
	gcc -ansi -pedantic -Wall -c stage_one_func.c
	
stage_two.o: stage_two.c stage_two.h 
	gcc -ansi -pedantic -Wall -c stage_two.c

conversions.o: conversions.c conversions.h
	gcc -ansi -pedantic -Wall -c conversions.c

clean:
	rm *.o