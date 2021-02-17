CC = gcc
CFLAGS = -Wall -pedantic -ansi -g -pthread
OBJ = lift_sim_A.o Queue.o
all: lift_sim_A lift_sim_B
lift_sim_A : $(OBJ)
	$(CC) $(OBJ) -lm -pthread -o lift_sim_A
lift_sim_A.o : lift_sim_A.c Queue.h
	$(CC) -c lift_sim_A.c $(CFLAGS)
lift_sim_B : lift_sim_B.o Queue.o
	$(CC) lift_sim_B.o Queue.o -lm -pthread -o lift_sim_B
lift_sim_B.o : lift_sim_B.c Queue.h
	$(CC) -c lift_sim_B.c $(CFLAGS)
Queue.o : Queue.c Queue.h
	$(CC) -c Queue.c $(CFLAGS)
clean:
	rm -f lift_sim_A lift_sim_B lift_sim_A.o lift_sim_B.o Queue.o
