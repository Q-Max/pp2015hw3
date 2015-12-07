 /* 
   Sequential Mandelbort sort
 */

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROOT 0

typedef struct complextype
{
	double real, imag;
} Compl;

int main(int argc, char** argv)
{
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;
	MPI_Request req;
	Display *display;
	Window window;      //initialization for a window
	int screen;         //which screen 

	const int n = atoi(argv[1]);
	const double xmin = atof(argv[2]);
	const double xmax = atof(argv[3]);
	const double ymin = atof(argv[4]);
	const double ymax = atof(argv[5]);
	const int width = atoi(argv[6]);
	const int height = atoi(argv[7]);
	const int disableX = strcmp("enable", argv[8]);
	const double xper = width/(xmax-xmin);
	const double yper = height/(ymax-ymin);
	GC gc;
	XGCValues values;
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
	int *repeats = (int*)malloc(sizeof(int)*width*height);
	double temp, lengthsq;
	int i, j, k, pernode = width/(size), retainer = width%(size);
	int *start = (int*)malloc(sizeof(int) * size);
	int *end = (int*)malloc(sizeof(int) * size);
	for(i=0;i<size;i++){
		if(retainer)  {
			if(i==0){
				start[i] = 0;
				end[i] = pernode + 1;
				retainer--;
			}
			else{
				start[i] = end[i-1];
				end[i] = start[i] + pernode + 1;
				retainer--;
			}
		}
		else{
			if(i==0){
				start[i] = 0;
				end[i] = pernode;
			}
			else{
				start[i] = end[i-1];
				end[i] = start[i] + pernode;
			}
		}
	}

	for(i=start[rank]; i<end[rank]; i++) {
		for(j=0; j<height; j++) {
			z.real = 0.0;
			z.imag = 0.0;
			c.real = ((double)i + xmin * xper)/xper; /* Theorem : If c belongs to M(Mandelbrot set), then |c| <= 2 */
			c.imag = ((double)j + ymin * yper)/yper; /* So needs to scale the window */
			repeats[i*height + j] = 0;
			lengthsq = 0.0;

			while(repeats[i*height + j] < 100000 && lengthsq < 4.0) { /* Theorem : If c belongs to M, then |Zn| <= 2. So Zn^2 <= 4 */
				temp = z.real*z.real - z.imag*z.imag + c.real;
				z.imag = 2*z.real*z.imag + c.imag;
				z.real = temp;
				lengthsq = z.real*z.real + z.imag*z.imag; 
				repeats[i*height + j]++;
			}
		}
	}
	if(rank==ROOT){
		for(j=1;j<size;j++){
			MPI_Recv(&repeats[height * start[j]], (end[j] - start[j])*height, MPI_INT, j, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		}
	}
	else{
		MPI_Send(&repeats[start[rank]*height],(end[rank] - start[rank])*height,MPI_INT,ROOT,0,MPI_COMM_WORLD);//,&req);
	}
	if(!disableX&&rank==ROOT) {		
		for(i=0;i<width;i++)
			for(j=0;j<height;j++){
				XSetForeground (display, gc,  1024 * 1024 * (repeats[i*height + j] % 256));
				XDrawPoint (display, window, gc, i, j);
			}
		XFlush(display);
		sleep(5);
	}
	if(!rank)
		puts("Finish");
	MPI_Finalize();
	return 0;
}