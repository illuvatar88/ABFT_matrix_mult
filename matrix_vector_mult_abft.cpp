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

#define SIZE_R 100
#define SIZE_C 100
#define SIZE_VECTOR SIZE_C
#define ITER 1
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
            #if 1
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
        C[0]=1;
        C[10]=2;
        C[16]=3;
        int *diff_vals_array = new int[SIZE_R];
        int diff_vals_count = 0;
        int sum_data = 0;
        int sum_dup = 0;
        #pragma omp parallel for shared(C, num_threads, diff_vals_array) private(i) reduction(+ : sum_data, sum_dup, diff_vals_count) schedule(static, (SIZE_R + 1) / num_threads)
        for (i = 0 ; i < SIZE_R ; i++) {
            int data = C[i * 2];
            int dup = C[i * 2 + 1];
            sum_data += data;
            sum_dup += dup;
            if (data != dup) {
                ++diff_vals_count;
                diff_vals_array[i] = 1;
            } else {
                diff_vals_array[i] = 0;
            }
        }
        int corrupted = 0;
        if (C[SIZE_R * 2] != sum_data) {
            corrupted = 1;
            if (C[SIZE_R * 2 + 1] == sum_dup) {
                #pragma omp parallel for shared(C, diff_vals_array) private(i) reduction(+ : sum_data, sum_dup, diff_vals_count) schedule(static, (SIZE_R + 1) / num_threads)
                for (i = 0 ; i < SIZE_R ; i++) {
                    if (diff_vals_array[i] == 1) {
                        C[i * 2] = C[i * 2 + 1];
                    }
                }

            } else {
                corrupted = 2;
            }
        }
        cout<<"Minimum Errors : "<<diff_vals_count<<endl;
        if (corrupted == 2) {
            cout<<"Data in C matrix irrecoverably corrupted\n";
        } else if (corrupted == 1) {
            cout <<"Data in C matrix recovered\n";
        } else {
            cout <<"C matrix intact\n";
        }
    }
    double end_time = timerval();
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
    double avg_run_time_us = (end_time - init_time) * 1e06 / ITER;
    cout<<"Average Run time: "<<avg_run_time_us<<" us\n";
    delete[] A, B, C;
    return 0;
}

