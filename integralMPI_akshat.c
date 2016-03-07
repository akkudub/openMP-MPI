#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include <xmmintrin.h>
#include <mpi.h>

double parts = 1000, four = 4.0, one = 1.0, result;

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
	srand(0);

	if (argc >= 2)
		parts = atoi(argv[1]);
	else
		parts = 1000;

	if (argc >= 3){
		printf("Incorrect number of arguments.");
		return 0;
	}


	long long after, before;
	int numprocs, myid, i;
	double gap = 0.0, sum = 0.0, local_result = 0.0, x;

	before = wall_clock_time();
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);

	// calculate in a round robin fashion
	gap = one / parts;
	sum = 0.0;
	for (i = myid + 1; i <= parts; i += numprocs) {
		x = gap * ((double)i - 0.5);
		sum += four / (one + x*x);
	}

	//combine into one sum for everything
	local_result = gap * sum;

	// reduce to result
	MPI_Reduce(&local_result, &result, 1, MPI_DOUBLE, MPI_SUM, 0,
			MPI_COMM_WORLD);

	if (myid == 0) {
		after = wall_clock_time();
		printf("Integrating with %d parts, it took %d processes to calculate %.10lfin %2.5f seconds\n", (int)parts, numprocs, result, ((float) (after - before)) / 1000000000);
	}

	MPI_Finalize();
} 
