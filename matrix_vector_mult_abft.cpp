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
#include <iomanip>
#include <limits>
#include <sched.h>
#include <cstring>
#include "timer.cpp"

using namespace std;

#define SIZE_R 10
#define SIZE_C 10
#define SIZE_VECTOR SIZE_C
#define ITER 10000
#define THREAD_LIMIT 2

int main (int argc, char *argv[]) {
    int max_threads = omp_get_num_procs();
    cout<<"Maximum no. of threads: "<<max_threads<<endl;
    int num_threads = (max_threads < THREAD_LIMIT ? max_threads : THREAD_LIMIT);
    omp_set_num_threads(num_threads);
    int *A = new int[(SIZE_R + 1) * SIZE_C];
    int *B = new int[SIZE_C * 2];
    int *C = new int[(SIZE_R + 1) * 2];
    int i, j, k;
    char thread_string[20];
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
        B[i * 2] = int(rand()) % 100;
        //B[i * 2] = 1;
    }
    double init_time = timerval();
    int *sum_c;
    for (int iter = 1; iter <= ITER ; iter++) {
        #pragma omp parallel shared(A) private(i, j, sum_c)
        {
            #pragma omp master
            {
                for (j = 0 ; j < SIZE_C ; j++) {
                    A[SIZE_R * SIZE_C + j] = 0;
                }
            }
            sum_c = new int[SIZE_C];
            for (j = 0 ; j < SIZE_C ; j++) {
                sum_c[j] = 0;
            }
            #pragma omp for schedule(static)
            for (i = 0 ; i < SIZE_R ; i++) {
                for (j = 0; j < SIZE_C ; j++) {
                    sum_c[j] += A[i * SIZE_C + j];
                }
            }
            #pragma omp critical
            {
                for (j = 0 ; j < SIZE_C ; j++) {
                    A[SIZE_R * SIZE_C + j] += sum_c[j];
                }
            }
            delete[] sum_c;
        }
        #pragma omp parallel for shared(B) private(i) schedule(static)
        for (i = 0 ; i < SIZE_C ; i++) {
            B[i * 2 + 1] = B[i * 2];
        }
        #pragma omp parallel shared(A, B, C, num_threads) private(i, j, k, thread_string)
            {
            #if 0
                if (iter == 1) {
                    sprintf(thread_string, "Thread: %d proc: %d\n", omp_get_thread_num(), sched_getcpu());
                    cout<<thread_string;
                }
            #endif
                #pragma omp for schedule(static, (SIZE_R + 1) / num_threads)
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
    }
    double end_time = timerval();
    #if 1
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
    double avg_run_time_us = (end_time - init_time) * 1e06 / ITER;
    cout<<"Average Run time: "<<avg_run_time_us<<" us\n";
    delete[] A, B, C;
    return 0;
}

