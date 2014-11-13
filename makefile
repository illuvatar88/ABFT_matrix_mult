all:
	g++ -fopenmp matrix_vector_mult_abft.cpp -o launch.out

run: all
	./launch.out
