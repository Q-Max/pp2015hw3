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
	$(MCC) MS_Hybrid_static.c -o MS_Hybrid_static $(CFLAG) -fopenmp
	echo "************************************************************"
	$(MCC) MS_Hybrid_dynamic.c -o MS_Hybrid_dynamic $(CFLAG) -pthread -fopenmp

	
clean:
	rm MS_seq MS_OpenMP_static MS_OpenMP_dynamic MS_MPI_static MS_MPI_dynamic MS_Hybrid_static MS_HYbrid_dynamic
test:
	time ./MS_OpenMP_static 4 -2 2 -2 2 800 800 enable
	time ./MS_OpenMP_dynamic 4 -2 2 -2 2 800 800 enable
	time mpiexec -np 4 ./MS_MPI_static 4 -2 2 -2 2 800 800 enable
	time mpiexec -np 4 ./MS_MPI_dynamic 4 -2 2 -2 2 800 800 enable
	time mpiexec -np 2 ./MS_Hybrid_static 2 -2 2 -2 2 800 800 enable
	time mpiexec -np 2 ./MS_Hybrid_dynamic 2 -2 2 -2 2 800 800 enable
