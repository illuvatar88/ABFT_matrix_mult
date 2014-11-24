all:
	g++ -static -g -fopenmp matrix_vector_mult_abft.cpp -o launch.out -D SIMPLIFY -O2

run: all
	./launch.out
