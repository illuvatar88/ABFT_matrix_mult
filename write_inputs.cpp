/*
 * write_inputs.cpp
 *
 *  Created on: 14-Dec-2014
 *      Author: sony
 */
/*
 * Matrix_vector_mult_abft.cpp
 *
 *  Created on: 26-Oct-2014
 *      Author: sony
 */


#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <iomanip>
#include <limits>
#include <cstring>
#include <fstream>
using namespace std;

#define SIZE_R 10000     //row size
#define SIZE_C 10000   //column size

int main (int argc, char *argv[]) {
    int *A = new int[SIZE_R * SIZE_C];    //input matrix
    int i, j;
    unsigned int seed = 123456789;
    srand(seed);
    //Assign input matrix
    for (i = 0 ; i < SIZE_R ; i++) {
        for (j = 0 ; j < SIZE_C ; j++) {
            A[i * SIZE_C + j] = int(rand()) % 10;
        }
    }
    ofstream fout ("input_matrix");
    if (fout.is_open()) {
        for (i = 0 ; i < SIZE_R ; i++) {
            for (j = 0 ; j < SIZE_C ; j++) {
                fout << A[i * SIZE_C + j] << " ";
            }
        }
    }
}




