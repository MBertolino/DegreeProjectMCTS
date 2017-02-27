#include <stdlib.h>
#include <stdio.h>

int main() {
	int N = 200000000;
	double x, y;
	int circle = 0;
	double rand_max2 = (double)RAND_MAX*(double)RAND_MAX;

	for (int i = 0; i < N; i++) {
		x = rand();
		y = rand();
		circle += (x*x + y*y)/rand_max2;
	}		
	float pi = 4.0*(N-circle)/((N));
	printf("pi = %f\n", pi);
		
	return 0;
}


