#include <stdio.h>
#include <time.h>
#include <mpi.h>
#include <xmmintrin.h>

long long wall_clock_time(){
#ifdef LINUX
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return (long long)(tp.tv_nsec + (long long)tp.tv_sec * 1000000000ll);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_usec * 1000 + (long long)tv.tv_sec * 1000000000ll);
#endif
}

int main(int argc, char** argv) {
    long long after, before;
    int numprocs, n, myid;
    double pi = 0.0, h, sum = 0.0, mypi = 0.0;
    double x;
    int i;



    /* before = wall_clock_time(); */
    /* MPI_Init(&argc, &argv); */
    /* MPI_Comm_size(MPI_COMM_WORLD, &nprocs); */
    /* MPI_Comm_rank(MPI_COMM_WORLD, &id); */

    /* for(int i = id + 1; i <= parts; i += nprocs) { */
    /*     int x = (i - 0.5) * interval; */
    /*     partial_sum += 4.0 / (1.0 + x * x); */
    /* } */

    /* MPI_Reduce(&partial_sum, &sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD); */
    /* if (id == 0) { */
    /*     sum *= interval; */
    /*     after = wall_clock_time(); */
        /* printf("Computing integral for %d parts using %d processes.\nIntegral value is %lf.\nIt took %2.4f seconds.\n", parts, nprocs, sum, ((float) (after - before)) / 1000000000); */
    /* } */
    /* MPI_Finalize(); */
    n = (argc >= 2) ? argv[1] : 1000;

    before = wall_clock_time();
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
  
	h   = 1.0 / (double) n;
	sum = 0.0;
	for (i = myid + 1; i <= n; i += numprocs) {
	    x = h * ((double)i - 0.5);
	    sum += 4.0 / (1.0 + x*x);
	}
	mypi = h * sum;
    
	MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0,
		   MPI_COMM_WORLD);
    
	if (myid == 0) {
        after = wall_clock_time();
        printf("%d, %d, %.16lf, %2.4f\n", n, numprocs, pi, ((float) (after - before)) / 1000000000);
	    /* printf("%.16f, %2.4f\n", */
		   /* pi, ((float) (after - before)) / 1000000000); */
    }
    MPI_Finalize();
} 
