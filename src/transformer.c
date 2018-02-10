#include <stdlib.h>
#include <math.h>
#include "transformer.h"

vec3 malloc_vec3(double a, double b, double c, vec3 res) {
    if (res == NULL)
        res = (vec3) malloc(3 * sizeof(double));
    res[0] = a;
    res[1] = b;
    res[2] = c;
    return res;
}

mat3 malloc_mat3(double a, double b, double c, double d, double e, double f, double g, double h, double i, mat3 res) {
    if (res == NULL)
        res = (mat3) malloc(9 * sizeof(double));
    res[0] = a;
    res[1] = b;
    res[2] = c;
    res[3] = d;
    res[4] = e;
    res[5] = f;
    res[6] = g;
    res[7] = h;
    res[8] = i;
    return res;
}

mat3 mul_mat3(mat3 a, mat3 b, mat3 res) {
    if (res == NULL)
        res = malloc_mat3(0,0,0,0,0,0,0,0,0,NULL);
    else
        res[0] = res[1] = res[2] = res[3] = res[4] = res[5] = res[6] = res[7] = res[8] = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                res[i*3+j] += a[i*3+k] * b[k*3+j];
    return res;
}

vec3 mul_mat3_vec3(mat3 a, vec3 b, vec3 res) {
    if (res == NULL)
        res = malloc_vec3(0,0,0,NULL);
    else
        res[0] = res[1] = res[2] = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            res[i] += a[i*3+j] * b[j];
    return res;
}

vec3 vec3_xy(double x, double y, vec3 res) {
    return malloc_vec3(x,y,1.00,res);
}

mat3 mat3_i(mat3 res) {
    return malloc_mat3(1,0,0,0,1,0,0,0,1,res);
}

mat3 mat3_translate(double x, double y, mat3 res) {
    return malloc_mat3(1,0,x,0,1,y,0,0,1,res);
}

mat3 mat3_scale(double x, double y, mat3 res) {
    return malloc_mat3(x,0,0,0,y,0,0,0,1,res);
}

mat3 mat3_rotate(double ang, mat3 res) {
    return malloc_mat3(cos(ang),-sin(ang),0,sin(ang),cos(ang),0,0,0,1,res);
}