#include "lsh.h"
#include "cluster.h"

using namespace std;

#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

double multiply_vectors(point, point);

double euclidean_distance(point, point);
double cosine_distance(point, point);
double distance(point, point, string);

void hamming_distance(vector<int>*, int, int, int, int);

int different(point, point);					//returns 1 if any difference in vectors found, 0 otherwise

int binary_search(double[], int, int, double);		//modified binary search, looks for num in range

double distance_sum(Data*, vector<Data*>, string);

#endif