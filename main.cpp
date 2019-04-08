#include "lsh.h"
#include "filedata.h"
#include "cluster.h"
#include "math_functions.h"

using namespace std;


int main (int argc, char* argv[]){
	int clusters= 5, hashfunctions = 5, tables = 5, max = 20, complete;
	int m = 100, probes = 10;
	string config;
	string input;
	string output = std::string("out");
	string metric = std::string("euclidean");
	command_line(argc, argv, &input, &config, &output, &metric, &complete);
	configuration(config, &clusters, &hashfunctions, &tables, &max);

  	std::ofstream out(output);
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf just to be safe
    std::cout.rdbuf(out.rdbuf());

	vector<Data> points;
	vector<Data> centroids;
	file_Data(&points, input);

	int vector_size = points[0].vector.size();
	HashTables hashtables(tables, points.size(), 8, hashfunctions, vector_size, metric);
	hashtables.insert(&points);

	hashfunctions = log2(points.size());
	int t = pow(2,hashfunctions);
	HyperCube cube(t, hashfunctions, vector_size, probes, m, metric);
	cube.insert_dataset(&points);

	run_algorithms(&points, &centroids, clusters, &hashtables, &cube, tables, hashfunctions, max, complete, metric);
	//init_assign_update(&points, &centroids, clusters, &hashtables, &cube, tables, hashfunctions, 1, 1, 1, max, complete, metric);
}