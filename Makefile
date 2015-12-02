CC = gcc
MCC = mpicc
CFLAG = -lX11 -lm -Wall -O3
all:
	$(CC) MS_seq.c -o MS_seq $(CFLAG)
	$(CC) MS_OpenMP_static.c -o MS_OpenMP_static $(CFLAG) -fopenmp
	$(CC) MS_OpenMP_dynamic.c -o MS_OpenMP_dynamic $(CFLAG) -fopenmp
	
clean:
	rm MS_seq MS_OpenMP_static MS_OpenMP_dynamic MS_MPI_static MS_MPI_dynamic MS_Hybird_static MS_HYbird_dynamic
