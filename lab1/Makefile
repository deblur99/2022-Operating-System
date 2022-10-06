CC = gcc

main.out : main.o input_data_handler.o input_validate.o
	gcc -o main.out main.c input_data_handler.c input_validate.c

main.o : main.c
	gcc -c main.c

input_data_handler.o : input_data_handler.c
	gcc -c input_data_handler.c

input_validate.o : input_validate.c
	gcc -c input_validate.c

clean :
	rm *.out *.o