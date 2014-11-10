all:
	export GOMP_CPU_AFFINITY=0,6
	g++ -fopenmp matrix_vector_mult_abft.cpp -o launch.out

run: all
	./launch.out
