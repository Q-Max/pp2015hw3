CC = gcc
MCC = mpicc
CFLAG = -lX11 -lm -Wall -O3
all:
	echo "************************************************************"
	$(CC) MS_seq.c -o MS_seq $(CFLAG)
	echo "************************************************************"
	$(CC) MS_OpenMP_static.c -o MS_OpenMP_static $(CFLAG) -fopenmp
	echo "************************************************************"
	$(CC) MS_OpenMP_dynamic.c -o MS_OpenMP_dynamic $(CFLAG) -fopenmp
	echo "************************************************************"
	$(MCC) MS_MPI_static.c -o MS_MPI_static $(CFLAG)
	echo "************************************************************"
	$(MCC) MS_MPI_dynamic.c -o MS_MPI_dynamic $(CFLAG) -pthread
	echo "************************************************************"
	$(MCC) MS_Hybird_static.c -o MS_Hybird_static $(CFLAG) -fopenmp
	echo "************************************************************"
	$(MCC) MS_Hybird_dynamic.c -o MS_Hybird_dynamic $(CFLAG) -pthread -fopenmp

	
clean:
	rm MS_seq MS_OpenMP_static MS_OpenMP_dynamic MS_MPI_static MS_MPI_dynamic MS_Hybird_static MS_HYbird_dynamic
