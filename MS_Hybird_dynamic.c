 /* 
   Sequential Mandelbort sort
 */

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <pthread.h>
#include <string.h>

#define ROOT 0
#define N_TIMES 40
#define TASK_TAG 0
#define SEND_INIT_TAG 1
#define SEND_COMP_TAG 2
#define TERMINATE_TAG 3
void *workPool(void* arg);
typedef struct complextype
{
	double real, imag;
} Compl;

int totalsize, pernode, retainer, height, width, size, disableX;
int *start, *end, *record, *repeats;
Display *display;
Window window;      //initialization for a window
GC gc;
XGCValues values;

int main(int argc, char** argv)
{
	int rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;
	MPI_Request req;
	
	int screen;         //which screen 
	pthread_t tid;
	//const int n = atoi(argv[1]);
	const double xmin = atof(argv[2]);
	const double xmax = atof(argv[3]);
	const double ymin = atof(argv[4]);
	const double ymax = atof(argv[5]);
	width = atoi(argv[6]);
	height = atoi(argv[7]);
	disableX = strcmp("enable", argv[8]);
	const double xper = width/(xmax-xmin);
	const double yper = height/(ymax-ymin);
	
	/* set window size */
	/*int width = 800;
	int height = 800;*/
	if(!disableX&&rank==ROOT){
		/* set window position */
		int x = 0;
		int y = 0;
		/* open connection with the server */ 
		display = XOpenDisplay(NULL);
		if(display == NULL) {
			fprintf(stderr, "cannot open display\n");
			return 0;
		}

		screen = DefaultScreen(display);
		/* border width in pixels */
		int border_width = 0;

		/* create window */
		window = XCreateSimpleWindow(display, RootWindow(display, screen), x, y, width, height, border_width,
						BlackPixel(display, screen), WhitePixel(display, screen));
	
		/* create graph */
		
		long valuemask = 0;
	
		gc = XCreateGC(display, window, valuemask, &values);
		//XSetBackground (display, gc, WhitePixel (display, screen));
		XSetForeground (display, gc, BlackPixel (display, screen));
		XSetBackground(display, gc, 0X0000FF00);
		XSetLineAttributes (display, gc, 1, LineSolid, CapRound, JoinRound);
	
		/* map(show) the window */
		XMapWindow(display, window);
		XSync(display, 0);
	}	
	/* draw points */
	Compl z, c;
	
	double temp, lengthsq;
	int i, j, k, max_i;
	pernode = width/(N_TIMES*size);
#ifdef DEBUG
	printf("pernode:%d\n",pernode);
#endif
	retainer = width%(N_TIMES*size);
	totalsize = N_TIMES*size;
	int *buf = malloc(sizeof(int)*((pernode+retainer)*height+1));
	int *buf2 = &buf[1];
	if(rank==ROOT)
		pthread_create(&tid, NULL, workPool, (void *) NULL);
	if(rank==ROOT) {
		pthread_join(tid, NULL);
		if(!disableX)
			XFlush(display);
		sleep(5);
		puts("Finish");
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Finalize();
		return 0;
	}

	do{
		MPI_Recv(&k, 1, MPI_INT, ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if(status.MPI_TAG==TERMINATE_TAG){
			break;
		}
		if(k==N_TIMES*size-1){
			max_i = (k+1)*pernode+retainer;
#ifdef DEBUG
			printf("i= %d, %d ,%d\n", k, ((k+1)*pernode), ((k+1)*pernode)+retainer);
#endif
		}
		else{
			max_i = ((k+1)*pernode);
		}
		buf[0] = k * pernode * height;
		#pragma omp parallel for private(i,j,z,c,lengthsq,temp)
		for(i=k*pernode;i<max_i;i++){
			for(j=0; j<height; j++) {
				z.real = 0.0;
				z.imag = 0.0;
				c.real = ((double)i + xmin * xper)/xper; /* Theorem : If c belongs to M(Mandelbrot set), then |c| <= 2 */
				c.imag = ((double)j + ymin * yper)/yper; /* So needs to scale the window */
				buf2[(i-k*pernode)*height + j] = 0;
				lengthsq = 0.0;

				while(buf2[(i-k*pernode)*height + j] < 100000 && lengthsq < 4.0) { /* Theorem : If c belongs to M, then |Zn| <= 2. So Zn^2 <= 4 */
					temp = z.real*z.real - z.imag*z.imag + c.real;
					z.imag = 2*z.real*z.imag + c.imag;
					z.real = temp;
					lengthsq = z.real*z.real + z.imag*z.imag; 
					buf2[(i-k*pernode)*height + j]++;
				}

			}
#ifdef DEBUG
			printf("rank: %d working %d times\n", rank, buf2[m*height + j]);
#endif
		}
		i = (max_i - (k * pernode)) * height;
		MPI_Isend(&i, 1, MPI_INT, ROOT, SEND_INIT_TAG, MPI_COMM_WORLD, &req);
		MPI_Isend(buf, i + 1, MPI_INT, ROOT, SEND_COMP_TAG, MPI_COMM_WORLD, &req);
	} while(1);	
	/*if(rank==ROOT)
		pthread_join(tid, NULL);
	if(!disableX&&rank==ROOT) {*/
		
		/*for(i=0;i<width;i++)
			for(j=0;j<height;j++){
				XSetForeground (display, gc,  1024 * 1024 * (repeats[i*height + j] % 256));
				XDrawPoint (display, window, gc, i, j);
			}*/
		/*XFlush(display);
		sleep(5);
	}
	if(!rank)
		puts("Finish");*/
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}

void *workPool(void* arg){
	
	repeats = (int*)malloc(sizeof(int)*width*height);
	int *buf = (int*)malloc(sizeof(int) * (height*(pernode+retainer) + 1));
	int *buf2 = &buf[1];
#ifdef DEBUG
	printf("buf : %x\n", buf);
	printf("%d\n", (height*(pernode+retainer+100) + 1));
#endif
	int i, j, count=0, working_slave=0, k;
	MPI_Status status;
	MPI_Request req;
	for(i=0;i<size-1;i++) {
		MPI_Isend(&i, 1, MPI_INT, i+1, TASK_TAG, MPI_COMM_WORLD, &req);
		count++;
		working_slave++;
	}
	
	while(working_slave!=0) {
#ifdef DEBUG
		printf("slaves = %d\n", working_slave);
#endif
		MPI_Recv(buf, 1, MPI_INT, MPI_ANY_SOURCE, SEND_INIT_TAG, MPI_COMM_WORLD, &status);
		// recv how much pixels
		j = buf[0];
		MPI_Recv(buf, j + 1, MPI_INT, status.MPI_SOURCE, SEND_COMP_TAG, MPI_COMM_WORLD, &status);
		// buf[0] is the location
		// recv data in buf[1] ~ buf[j]
		
		
		working_slave--;
		if(count<totalsize) {
			k = count;
			MPI_Isend(&k, 1, MPI_INT, status.MPI_SOURCE, TASK_TAG, MPI_COMM_WORLD, &req);
#ifdef DEBUG
			printf("count = %2d   slave = %2d\n",k, status.MPI_SOURCE);
#endif
			count++;
			working_slave++;
		}
		else if(count==totalsize){
			for(i=0;i<size;i++){
				MPI_Isend(&count, 1, MPI_INT, i, TERMINATE_TAG, MPI_COMM_WORLD, &req);
				//MPI_Request_free(&req);
			}
			count++;
		}
		if(!disableX){
			for(i=0;i<j;i++){
				XSetForeground (display, gc,  1024 * 1024 * (buf2[i] % 256));
				XDrawPoint (display, window, gc, (buf[0]+i)/height, i%height);
			}
		}
		// if enableX, directly draw to window
		else{
			memcpy(&repeats[buf[0]], &buf[1], sizeof(int) * j);
			// copy to repeats		
		}
	}
	pthread_exit(0);
}
