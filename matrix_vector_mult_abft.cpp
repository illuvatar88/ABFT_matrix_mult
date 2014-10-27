/*
 * Matrix_vector_mult_abft.cpp
 *
 *  Created on: 26-Oct-2014
 *      Author: sony
 */

#include <omp.h>
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include "timer.cpp"

using namespace std;

#define SIZE_R 1000
#define SIZE_C 100
#define SIZE_VECTOR SIZE_C
#define ITER 10000

int main (int argc, char *argv[]) {
    int max_threads = omp_get_num_procs();
    int num_threads = (max_threads < SIZE_R ? max_threads : SIZE_R);
    cout<<"Maximum no. of threads: "<<num_threads<<endl;
    omp_set_num_threads(num_threads);
    int *A = new int[SIZE_R * SIZE_C];
    int *B = new int[SIZE_C];
    int *C = new int[SIZE_R];
    int i, j;
    unsigned int seed = 123456789;
    srand(seed);
#pragma omp parallel for shared(A) private(i, j) schedule(static)
    for (i = 0 ; i < SIZE_R ; i++) {
        for (j = 0 ; j < SIZE_C ; j++) {
            A[i * SIZE_C + j] = int(rand()) % 100;
            //A[i * SIZE_C + j] = 1;
        }
    }
#pragma omp parallel for shared(B) private(i) schedule(static)
    for (i = 0 ; i < SIZE_C ; i++) {
        B[i] = int(rand()) % 100;
        //B[i] = 1;
    }
    double init_time = timerval();
    for (int iter = 1; iter <= ITER ; iter++) {
    #pragma omp parallel for shared(A, B, C) private(i, j) schedule(static)
        for (i = 0 ; i < SIZE_R ; i++) {
            C[i] = 0;
            for (j = 0 ; j < SIZE_C ; j++) {
                C[i] += A[i * SIZE_C + j] * B[j];
            }
        }
    }
    double end_time = timerval();
#if 0
    cout<<"Input matrix A :\n";
    for (i = 0 ; i < SIZE_R ; i++) {
        for (j = 0 ; j < SIZE_C ; j++) {
            cout<<A[i * SIZE_C + j]<<" ";
        }
        cout<<endl;
    }
    cout<<"Input vector B :\n";
    for (i = 0 ; i < SIZE_C ; i++) {
        cout<<B[i]<<" ";
    }
    cout<<endl;
    cout<<"Output vector C: \n";
    for (i = 0 ; i < SIZE_R ; i++) {
        cout<<C[i]<<" ";
    }
    cout<<endl;
#endif
    cout<<end_time<<" "<<init_time<<endl;
    double avg_run_time_us = (end_time - init_time) * 1e06 / ITER;
    cout<<"Average Run time: "<<avg_run_time_us<<" us\n";
    return 0;
}

