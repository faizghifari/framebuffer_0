#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include "image.h"

typedef double* vec3;
typedef double* mat3;

vec3 malloc_vec3(double a, double b, double c, vec3 res);

mat3 malloc_mat3(double a, double b, double c, double d, double e, double f, double g, double h, double i, mat3 res);

mat3 mul_mat3(mat3 a, mat3 b, mat3 res);

vec3 mul_mat3_vec3(mat3 a, vec3 b, vec3 res);

vec3 vec3_xy(double x, double y, vec3 res);

mat3 mat3_i(mat3 res);

mat3 mat3_translate(double x, double y, mat3 res);

mat3 mat3_scale(double x, double y, mat3 res);

mat3 mat3_rotate(double ang, mat3 res);

#endif