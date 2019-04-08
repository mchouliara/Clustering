#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>   //istringstream
#include <random>   //for normal distribution
#include  <cstdlib>	//for pseudo-random rand function
#include <ctime>
#include <algorithm>
#include <unordered_map>
using namespace std;

#ifndef LSH_H
#define LSH_H

#define W 0.3


typedef vector<double> point;


class Data {
public:
	string item;
	point vector;
	int centroid;
	double dist;
	int range; 		//for range assignment
	Data(){}
	void set(string s, point p) {item = s; vector = p; centroid = -1; dist = -1; range = 0;}
};

class Neighbor {					//for neighbor computations
public:
	Data* data;
	double dist;	
};


class H1{							// (vector v, t) for euclidean computations
public:								
	point v;						// t is disregarded for cosine LSH
	double t;

	H1(){}
	H1(point, double);
	//~H1();
} ;

class H2{						//all hash function info of a HashTable
public:
	H1* hash_data;
	int* r_vector;				//r is also disregarded for cosine LSH
	int hash_num;
	H2(int);
	void set(int);
	void print();
	int memory();
	~H2();
};

class HashTable{
protected:
	int table_size;
	H2* hash_methods;
	vector<Data *> * buckets;
public:
	HashTable(int, int, int);
	void print();
	~HashTable();
};

class Euclidean_HT : public HashTable
{
	vector<string> * g_functions;			//identical to the hashtable buckets
public:
	Euclidean_HT(int b, int k, int d) : HashTable(b, k, d)
	{
		g_functions = new vector<string>[b];
	}
	void euclidean(Data*, string*, int*);
	void insert(Data*);
	Neighbor* nearest_neighbor(Data *);
	vector<Neighbor>*  range_search(Data *, double);
	int memory();
	~Euclidean_HT()
	{
		delete[] g_functions;
	}
};

class Cosine_HT : public HashTable
{
public:
	Cosine_HT(int b, int k, int d) : HashTable(b, k, d){}
	int cosine(Data*);
	void insert(Data*);
	Neighbor* nearest_neighbor(Data *);
	vector<Neighbor> * range_search(Data *, double);
	int memory();
};

class HashTables
{
public:
	Euclidean_HT** Euc_HT;
	Cosine_HT** Cos_HT;
	string metric;
	int tables;
	HashTables(int ntables, int points, int divisor, int hashfunctions, int vector_size, string type)
	{
		int t;
		metric = type;
		tables = ntables;
		if(metric == std::string("euclidean"))
		{
			cout << right << endl;
			t = points/divisor;
			Euc_HT = new Euclidean_HT*[tables];
			for(int i = 0; i < tables; i++)
			{
				Euc_HT[i] = new Euclidean_HT(t, hashfunctions, vector_size);
			}
		}
		else if(metric == std::string("cosine"))
		{
			t = pow(2, hashfunctions);
			Cos_HT = new Cosine_HT*[tables];
			for(int i = 0; i < tables; i++)
			{
				Cos_HT[i] = new Cosine_HT(t, hashfunctions, vector_size);
			}
		}
	}

	void insert(vector<Data>* points)
	{
		if(metric == std::string("euclidean"))
		{
			for(int i = 0; i < points->size() ; i++)
			{
				for(int j = 0; j < tables; j++)
				{
					Euc_HT[j]->insert(&(points->at(i)));
				}
			}
		}else if(metric == std::string("cosine"))
		{
			for(int i = 0; i < points->size() ; i++)
			{
				for(int j = 0; j < tables; j++)
				{
					Cos_HT[j]->insert(&(points->at(i)));
				}
			}
		}		
	}

	~HashTables()
	{
		if(metric == std::string("euclidean"))
		{
			for(int i = 0; i < tables; i++)
			{
				delete Euc_HT[i];
			}
			delete[] Euc_HT;
		}
		else if(metric == std::string("cosine"))
		{
			for(int i = 0; i < tables; i++)
			{
				delete Cos_HT[i];
			}
			delete[] Cos_HT;
		}	
	}
};

class HyperCube : public HashTable
{
	int maxneighbors;
	int probes;
	string metric;
	vector<int> dictionary[2];				//translating euclidean g into 0s and 1s
public:
	HyperCube(int b, int k, int d, int p, int m, string met) : HashTable(b, k, d){
		probes = p;
		maxneighbors = m;
		metric = met;
	}
	int find(int);
	int cube_pos(Data *);
	void insert(Data*);
	void insert_dataset(vector<Data>*);
	double nearest_neighbor(Data *, int, float*);
	vector<Neighbor>* range_search(Data, int, double);
	int memory();
};

//double Real_NN(vector<Data>, Data, string);  		//returns distance
//double NearestN(void*, Data, int, string, float);			//returns distance
vector<Neighbor>* RangeSearch(HashTables*, Data, int, double, string);
//int Memory(vector<Data>, void*, string, int);
//void Query(vector<Data>, void*, string, int, double, string);
void Cube_range(HyperCube*, string, int, double);


#endif
