/*
 * checksum.cpp
 *
 *  Created on: Nov 3, 2014
 *      Author: arunkumar
 */

#include <omp.h>
#include <iostream>

using namespace std;

#define ERROR_RATIO 0.0005

int calc_column_checksum (int A[], int size_r, int size_c, int num_threads);
int dup_column_vector (int B[], int size, int num_threads);
void inject_random_error (int C[], int size_r, int size_c);
int check_vector_checksum (int C[], int size, int num_threads, int &num_errors, int &corrupted);

int calc_column_checksum (int A[], int size_r, int size_c, int num_threads) {
    int i, j;
    int *sum_c;
    #pragma omp parallel shared(A) private(i, j, sum_c)
    {
        #pragma omp master
        {
            for (j = 0 ; j < size_c ; j++) {
                A[size_r * size_c + j] = 0;
            }
        }
        sum_c = new int[size_c];
        for (j = 0 ; j < size_c ; j++) {
            sum_c[j] = 0;
        }
        #pragma omp for schedule(static, (size_r + 1) / num_threads)
        for (i = 0 ; i < size_r ; i++) {
            for (j = 0; j < size_c ; j++) {
                sum_c[j] += A[i * size_c + j];
            }
        }
        #pragma omp critical
        {
            for (j = 0 ; j < size_c ; j++) {
                A[size_r * size_c + j] += sum_c[j];
            }
        }
        delete[] sum_c;
    }
    return 0;
}

int dup_column_vector (int B[], int size, int num_threads) {
    int i;
    #pragma omp parallel for shared(B) private(i) schedule(static, (size + 1) / num_threads)
    for (i = 0 ; i < size ; i++) {
        B[i * 2 + 1] = B[i * 2];
    }
    return 0;
}

int check_vector_checksum (int C[], int size, int num_threads, int &num_errors, int &corrupted) {
    int *diff_vals_array = new int[size];
    int errors = 0;
    int sum_data = 0;
    int sum_dup = 0;
    int i;
    #pragma omp parallel for shared(C, num_threads, diff_vals_array) private(i) reduction(+ : sum_data, sum_dup, errors) schedule(static, (size + 1) / num_threads)
    for (i = 0 ; i < size ; i++) {
        int data = C[i * 2];
        int dup = C[i * 2 + 1];
        sum_data += data;
        sum_dup += dup;
        if (data != dup) {
            ++errors;
            diff_vals_array[i] = 1;
        } else {
            diff_vals_array[i] = 0;
        }
    }
    corrupted = 0;
    if (C[size * 2] != sum_data) {
        if (C[size * 2 + 1] == sum_dup) {
            #pragma omp parallel for shared(C, diff_vals_array) private(i) schedule(static, (size + 1) / num_threads)
            for (i = 0 ; i < size ; i++) {
                if (diff_vals_array[i] == 1) {
                    C[i * 2] = C[i * 2 + 1];
                }
            }
        } else {
            corrupted = 1;
        }
    } else {
        errors = 0;
    }
    num_errors = errors;
    return 0;
}

void inject_random_error (int C[], int size_r, int size_c) {
    int num_errors = float(size_r * size_c) * float(ERROR_RATIO);
    num_errors = (num_errors > 0 ? num_errors : 1);
    unsigned int seed = 123456789;
    srand(seed);
    int i;
    #pragma omp parallel for shared(C) private(i) schedule(static)
    for (i = 0 ; i < num_errors ; i++) {
        int index = rand() % (size_r * size_c);
        C[index] = rand() % 100;
    }
}
