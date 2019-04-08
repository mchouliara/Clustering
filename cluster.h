#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <random>   //for normal distribution
#include  <cstdlib>	//for pseudo-random rand function
#include "lsh.h"

using namespace std;

#ifndef CLUSTER_H
#define CLUSTER_H

typedef struct Nearest_Centroid
{
	int centroid;
	double dist;
} Cluster_info;

//void clear_all(vector<Data>*, vector<Data>*)

int is_centroid(Data, vector<Data>);

Nearest_Centroid* min_centroid(Data, vector<Data>, string);
double centroid_dist(Data, vector<Data>, string);				//same as above, but returns only distance


void reset_points(vector<Data>*);							//wipe previous centroid info
double compute_range(vector<Data>, int);					//set initial r = min distance between centers/n
void unassigned_points(vector<Data>*, vector <Data>);		//initialize all uninitialized values + make sure centroids are in their own cluster

void random_init(vector <Data>, vector<Data>*, int);
void k_means_init(vector <Data>, vector<Data>*, int, string);
void initialize(vector<Data>, vector<Data>*, int, int, string);


int lsh_replace(vector<Data>*, vector<Neighbor>*, int, int); 				//returns >0 if any nearest centroids changed, 0 if all unchanged
void lloyd_assign(vector<Data>*, vector<Data>, string);
void lsh_assign(vector<Data>*, vector<Data>, HashTables*, int, string);
void hypercube_assign(vector<Data>*, vector<Data>, HyperCube*, int, string);
void assign(vector<Data>*, vector<Data>, HashTables*, HyperCube*, int, int, int, string);

int k_means_update(vector<Data>, vector<Data>*);			//0 if no centroids changed, 1 if any centroid change
int pam_update(vector<Data>, vector<Data>*, string);
int update(vector<Data>, vector<Data>*, int, string);

int second_best(Data, int, vector<Data>, string);		//returns pos of 2nd best centroid | second_best(point, current centroid it belongs to, all centroids)

string algorithm_name(int, int, int);

double silhouette(vector<Data>, vector<Data>, int, string);
void init_assign_update(vector<Data>* points, vector<Data>* centroids, int clusters, HashTables* hashtables, HyperCube* cube, int tables, int hashfunctions, int init, int assign, int update, int max, int complete, string type); //gia atomika run algorithmwn

void run_algorithms(vector<Data>* points, vector<Data>* centroids, int clusters, HashTables* hashtables, HyperCube* cube, int tables, int hashfunctions, int max, int complete, string type);
#endif