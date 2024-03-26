#ifndef _CUSTOM_ARRAY_H_
#define _CUSTOM_ARRAY_H_

#include <stdio.h>
#include <stdlib.h>

namespace customarray {
    double*
    initialize(double val)
    {
        double *arr = (double*)malloc(sizeof(double));
        arr[0] = val;
        return arr;
    }

    double*
    insert(double *arr, double val)
    {
        arr = (double*)realloc(arr, sizeof(double)*(arr[0] + 1));
        *(++arr) = val;
        return arr;
    }

    void free(double *arr)
    {
        delete [] arr;
    }
}

#endif