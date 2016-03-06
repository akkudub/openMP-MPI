#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include <omp.h>
#include <xmmintrin.h>

long double one = 1.0;
long double four = 4.0;

long double parts = 1000;
int threads = -1;
long double result;

typedef struct{
	long double numerator;
	long double denominator;
} fraction;

fraction* fraction_results;

fraction final_result;


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

void work_accurate(){
	fraction_results = (fraction *)malloc(sizeof(fraction)*parts);

	int i, j, k, l;

	// Parallelize the integration
	// first resolve all the denominators
	// #pragma omp parallel for reduction(+:fraction_results)
#pragma omp parallel for
	for (i = 0; i < (int)parts; i++){
		// not dividing by the fraction first, will accumulate all then divide by the number of parts
		long double local_denominator = (parts*parts)+(i*i);;
		// accumulate all the partial sums
		fraction_results[i].denominator = local_denominator;
	}

	// calculate all the individual numerators
#pragma omp parallel for
	for (j = 0; j < (int)parts; j++){
		long double local_numerator = 1;
		for (k = 0; k < (int)parts; k++){
			// get the numerators for 
			if (j != k){
				local_numerator *= fraction_results[k].denominator;
			}
		}		
		// accumulate all the partial sums
		fraction_results[j].numerator = local_numerator;
	}
	// get the final result by adding all the numerators and multiplying all the denominators
	for (l = 0; l < parts; l++){
		final_result.numerator += fraction_results[i].numerator;
		final_result.denominator *= fraction_results[i].denominator;
	}

}

void work_fast(){
	// fraction_results = (fraction *)malloc(sizeof(fraction)*parts);
	int i;	
	#pragma omp parallel for reduction (+:result)
	for(i=0; i<(int)parts; i++){
		long double local = (four * parts)/((parts * parts)+ ((long double)i*(long double)i));
		result+=local;
	}
}


int main(int argc, char ** argv){
	srand(0);

	if (argc >= 2)
		parts = atoi(argv[1]);
	else
		parts = 1000;

	if (argc >= 3)
		threads = atoi(argv[2]);
	else
		threads = -1;

	if (argc >=4){
		printf("Incorrect number of arguments.");
		return 0;
	}

	// Multiply the matrices
	if (threads != -1)
	{
		omp_set_num_threads(threads);
	}

#pragma omp parallel
	{
		threads = omp_get_num_threads();
	}

	long long before, after;
	before = wall_clock_time();
	//work_accurate();
	work_fast();
	after = wall_clock_time();
	//printf("Using %d threads, integral is %Lf ", threads, (final_result.numerator/final_result.denominator) * ( parts * parts * 4));
	printf("Using %d threads, integral is %2.10Lf \n", threads,result);


	fprintf(stderr, "Integration took %2.4f seconds\n", ((float)(after - before))/1000000000);

	return 0;
}
