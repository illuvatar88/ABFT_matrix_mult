run:
	export GOMP_CPU_AFFINITY=0,1
	g++ -fopenmp matrix_vector_mult_abft.cpp

all: run