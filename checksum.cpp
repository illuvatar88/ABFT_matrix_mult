/*
 * checksum.cpp
 *
 *  Created on: Nov 3, 2014
 *      Author: arunkumar
 */

#include <omp.h>
#include <iostream>

using namespace std;

#define ERROR_RATIO 0.5 //used by inject_random_error for fraction of cells to corrupt

int calc_column_checksum (int A[], int size_r, int size_c, int num_threads);
int dup_column_vector (int B[], int size, int num_threads);
void inject_random_error (int C[], int size_r, int size_c);
int check_vector_checksum (int C[], int size, int num_threads, int &num_errors, int &corrupted);
int check_matrix_checksum (int A[], int size_r, int size_c, int &num_errors, int &corrupted);
int calc_matrix_checksum (int A[], int size_r, int size_c);

/*
//calculate column checksum
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
*/

//calculate column checksum
int calc_column_checksum (int A[], int size_r, int size_c, int num_threads) {
    int i, j;
    int *sum_c;
    for (j = 0 ; j < size_c ; j++) {
        A[size_r * size_c + j] = 0;
    }
    for (i = 0 ; i < size_r ; i++) {
        for (j = 0; j < size_c ; j++) {
            A[size_r * size_c + j] += A[i * size_c + j];
        }
    }
    return 0;
}


//duplicate column for vector
int dup_column_vector (int B[], int size, int num_threads) {
    int i;
    //#pragma omp parallel for shared(B) private(i) schedule(static, (size + 1) / num_threads)
    for (i = 0 ; i < size ; i++) {
        B[i * 2 + 1] = B[i * 2];
    }
    return 0;
}

/*
//check checksum and correct if possible
int check_vector_checksum (int C[], int size, int num_threads, int &num_errors, int &corrupted) {
    int *diff_vals_array = new int[size];
    int errors = 0;
    long int sum_data = 0;
    long int sum_dup = 0;
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
    cout << "Column sum : " << sum_data << endl;
    cout << "Column checksum : " << C[size * 2] << endl;
    corrupted = 0;
    if (C[size * 2] != sum_data) {
        ++errors;
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
    }
    num_errors = errors;
    return 0;
}
*/

//check checksum and correct if possible
int check_vector_checksum (int C[], int size, int num_threads, int &num_errors, int &corrupted) {
    int *diff_vals_array = new int[size];
    int errors = 0;
    long int sum_data = 0;
    long int sum_dup = 0;
    int i;
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
    cout << "Column sum : " << sum_data << endl;
    cout << "Column checksum : " << C[size * 2] << endl;
    corrupted = 0;
    if (C[size * 2] != sum_data) {
        ++errors;
        if (C[size * 2 + 1] == sum_dup) {
            for (i = 0 ; i < size ; i++) {
                if (diff_vals_array[i] == 1) {
                    C[i * 2] = C[i * 2 + 1];
                }
            }
        } else {
            corrupted = 1;
        }
    }
    num_errors = errors;
    return 0;
}


//calculate matrix checksum...size_r and size_c are row and column sizes excluding the checksum
int calc_matrix_checksum (int A[], int size_r, int size_c) {
    int i, j;
    //initialize column checksums
    for (j = 0 ; j < size_c + 1 ; j++) {
        A[size_r * (size_c + 1) + j] = 0;
    }
    //initialize Row checksums
    for (i = 0 ; i < size_r ; i++) {
        A[size_c + (size_c + 1) * i] = 0;
    }
    for (i = 0 ; i < size_r ; i++) {
        for (j = 0; j < size_c ; j++) {
            A[size_c + (size_c + 1) * i] += A[i * (size_c + 1) + j];
            A[size_r * (size_c + 1) + j] += A[i * (size_c + 1) + j];
        }
            A[(size_r + 1) * (size_c + 1) -1] += A[size_c + (size_c + 1) * i];
    }
    return 0;
}


//check checksum and correct if possible....size_r and size_c are total matrix dimensions
int check_matrix_checksum (int A[], int size_r, int size_c, int &num_errors, int &corrupted) {
    int errors = 0;
    int *sum_c;
    int *sum_r = new int[size_r];
    int i, j;
    sum_c = new int[size_c];
    for (j = 0 ; j < size_c ; j++) {
        sum_c[j] = 0;
    }
    for (i = 0 ; i < size_r ; i++) {
        sum_r[i] = 0;
    }
    for (i = 0 ; i < size_r - 1 ; i++) {
        for (j = 0; j < size_c - 1 ; j++) {
            sum_r[i] += A[i * size_c + j];
            sum_c[j] += A[i * size_c + j];
        }
        sum_c[size_c - 1] += A[size_c - 1 + size_c * i];
    }
    int error_pos_c = -1;
    int error_pos_r = -1;
    for (j = 0 ; j < size_c ; j++) {
        if (sum_c[j] != A[(size_r - 1) * size_c + j]) {
            error_pos_c = j;
            ++errors;
        }
    }
    for (i = 0 ; i < size_r - 1 ; i++) {
        if (sum_r[i] != A[size_c * i + size_c - 1]) {
            error_pos_r = i;
            ++errors;
        }
    }
    num_errors = errors;
    corrupted = 0;
    if (errors == 2 && error_pos_r != -1 && error_pos_c != -1) {
        cout << "Erroneous element : " << A[size_c * error_pos_r + error_pos_c];
        A[size_c * error_pos_r + error_pos_c] = A[size_c * error_pos_r + size_c - 1] - (sum_r[error_pos_r] - A[size_c * error_pos_r + error_pos_c]);
        cout << " corrected to : " << A[size_c * error_pos_r + error_pos_c] << endl;
    } else if (errors > 2) {
        corrupted = 1;
    }
    return 0;
}


void inject_random_error (int C[], int size_r, int size_c) {
    int num_errors = float(size_r * size_c) * float(ERROR_RATIO);
    num_errors = (num_errors > 0 ? num_errors : 1);
    //unsigned int seed = 123456789;
    //srand(seed);
    int i;
    #pragma omp parallel for shared(C) private(i) schedule(static)
    for (i = 0 ; i < num_errors ; i++) {
        int index = rand() % (size_r * size_c);
        C[index] = rand() % 100;
    }
}
