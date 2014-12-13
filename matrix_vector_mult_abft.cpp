/*
 * Matrix_vector_mult_abft.cpp
 *
 *  Created on: 26-Oct-2014
 *      Author: sony
 */

//Begin SimPLiFI Header
//errorDetection=error_count.dat
//start=matrix_vector_mult
//end=matrix_vector_mult
//output=result.dat
//End SimPLiFI Header

#include <omp.h>
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <iomanip>
#include <limits>
#include <sched.h>
#include <cstring>
#include "timer.cpp"
#include "checksum.cpp"
using namespace std;

#define SIZE_R 1024     //row size
#define SIZE_C 1024   //column size
#define SIZE_VECTOR SIZE_C      //vector length
#define ITER 1      //number of iterations
#define THREAD_LIMIT 2      //max number of threads to use

#ifdef SIMPLIFI
#include "SimPLiFI_sighandlers.h"
#endif

void matrix_vector_mult(int *A, int *B, int *C, int num_threads);

int main (int argc, char *argv[]) {
#ifdef SIMPLIFI
    SimPLiFI_register_sighandlers();
#endif
    int max_threads = omp_get_num_procs();
    int num_threads = (max_threads < THREAD_LIMIT ? max_threads : THREAD_LIMIT);
    omp_set_num_threads(num_threads);
    int *A = new int[(SIZE_R + 1) * SIZE_C];    //input matrix
    int *B = new int[SIZE_C * 2];    //input vector
    int *C = new int[(SIZE_R + 1) * 2];    //output matrix
    int i, j, k;
    unsigned int seed = 123456789;
    srand(seed);
    //Block to check affinity
#if 0
    #pragma omp parallel
    {
        char thread_string[20];
        sprintf(thread_string, "Thread: %d proc: %d\n", omp_get_thread_num(), sched_getcpu());
        cout<<thread_string;
    }
#endif
    //Assign input matrix
    //#pragma omp parallel for shared(A) private(i, j) schedule(static)
    for (i = 0 ; i < SIZE_R ; i++) {
        for (j = 0 ; j < SIZE_C ; j++) {
            A[i * SIZE_C + j] = int(rand()) % 10;
        }
    }
    //Assign input vector
    //#pragma omp parallel for shared(B) private(i) schedule(static)
    for (i = 0 ; i < SIZE_C ; i++) {
        B[i * 2] = int(rand()) % 10;
    }
    double init_time = timerval();    //start timing
    int errors, corrupted;
    //checksum for matrix A
    if (calc_column_checksum(A, SIZE_R, SIZE_C, num_threads) != 0) {
        return 1;
    }
    //vector B duplicated
    if (dup_column_vector(B, SIZE_C, num_threads) != 0) {
        return 1;
    }
    double init_time_op = timerval();
    //run multiplication
    matrix_vector_mult(A, B, C, num_threads);
    double end_time_op = timerval();
#if 0   //set to 1 to use random error injection function in checksum.cpp
        inject_random_error(C, SIZE_R + 1, 2);
#endif
    //check and try to recover otherwise set corrupted as 1
    if (check_vector_checksum(C, SIZE_R, num_threads, errors, corrupted) != 0) {
        return 1;
    }
    double end_time = timerval();    //end timing
    if (errors == 0) {
        cout << "Vector Intact\n";
    } else {
        if (corrupted == 0) {
            cout << "Vector recovered, corrected errors : "<<errors<<endl;
        } else {
            cout << "Vector Corrupted with minimum errors : "<<errors<<endl;
        }
    }
    //Block to print input and output
#if 0
    cout<<"Input matrix A :\n";
    for (i = 0 ; i <= SIZE_R ; i++) {
        for (j = 0 ; j < SIZE_C ; j++) {
            cout<<A[i * SIZE_C + j]<<" ";
        }
            cout<<endl;
    }
    cout<<"Input vector B :\n";
    for (i = 0 ; i < SIZE_C ; i++) {
        cout<<B[i * 2]<<" "<<B[i * 2 + 1]<<endl;
    }
    cout<<"Output vector C: \n";
    for (i = 0 ; i <= SIZE_R ; i++) {
        cout<<C[i * 2]<<" "<<C[i * 2 + 1]<<endl;
    }
#endif
    //cout<<setprecision(numeric_limits<double>::digits10 + 1)<<end_time<<" "<<init_time<<endl;
    double avg_run_time_op_us = (end_time_op - init_time_op) * 1e06 / ITER;
    double avg_run_time_us = (end_time - init_time) * 1e06 / ITER;
    double overhead = (avg_run_time_us - avg_run_time_op_us) / avg_run_time_op_us * 100;
    cout<<"Average Run time op, Average Run time, overhead : "<<avg_run_time_op_us<<" us, "<<avg_run_time_us<<" us, "<<overhead<<" %\n";
    FILE *err_cnt=fopen("error_count.dat", "w");
    FILE *result=fopen("result.dat", "w");
    //save result of ABFT check (used in validation program)
    fprintf(err_cnt, "%d\n", errors);
    //save C matrix
    for(i = 0; i < SIZE_R; i++) {
            fprintf(result, "%011d,    ", C[i * 2]);
            fprintf(result, ";\n");
    }
    delete[] A, B, C;
    return 0;
}

void matrix_vector_mult(int *A, int *B, int *C, int num_threads) {
    int i, j, k;
    #pragma omp parallel for shared(A, B, C, num_threads) private(i, j, k) schedule(static, (SIZE_R + 1) / num_threads)
    for (i = 0 ; i <= SIZE_R ; i++) {
        for (j = 0 ; j < 2 ; j++) {
            int sum = 0;
            for (k = 0 ; k < SIZE_C ; k++) {
                sum += A[i * SIZE_C + k] * B[k * 2 + j];
            }
            C[i * 2 + j] = sum;
        }
    }
}
