CC = gcc
MCC = mpicc
CFLAG = -lX11 -lm -Wall -O3
all:
	echo "MS_seq"
	$(CC) MS_seq.c -o MS_seq $(CFLAG)
	echo "MS_OpenMP_static"
	$(CC) MS_OpenMP_static.c -o MS_OpenMP_static $(CFLAG) -fopenmp
	echo "MS_OpenMP_dynamic"
	$(CC) MS_OpenMP_dynamic.c -o MS_OpenMP_dynamic $(CFLAG) -fopenmp
	echo "MS_MPI_static"
	$(MCC) MS_MPI_static.c -o MS_MPI_static $(CFLAG)
	echo "MS_MPI_dynamic"
	$(MCC) MS_MPI_dynamic.c -o MS_MPI_dynamic $(CFLAG) -pthread
	
clean:
	rm MS_seq MS_OpenMP_static MS_OpenMP_dynamic MS_MPI_static MS_MPI_dynamic MS_Hybird_static MS_HYbird_dynamic
