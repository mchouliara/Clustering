#include "math_functions.h"
#include "cluster.h"
using namespace std;

int is_centroid(Data point, vector<Data> centroids)
{
	for(int i = 1; i < centroids.size(); i++)
	{
		if(point.item == centroids[i].item)
		{
			return i;
		}
	}
	return -1;
}

Nearest_Centroid* min_centroid(Data point, vector<Data> centroids, string metric)
{
	if(centroids.size() == 0)
	{
		return nullptr;
	}
	Nearest_Centroid* min = new Nearest_Centroid;
	int flag = is_centroid(point, centroids);
	if(flag > 0)
	{
		min->dist = 0.0;
		min->centroid = flag;
		//cout << point.item << " is a centre" << endl;
		return min;
	}
	double dist;
	min->dist = distance(point.vector, centroids[0].vector, metric);
	min->centroid = 0;
	for(int i = 1; i < centroids.size(); i++)
	{
		dist = distance(point.vector, centroids[i].vector, metric);
		if(dist < min->dist)
		{
			min->dist = dist;
			min->centroid = i;
		}
	}
	return min;
}

int second_best(Data point, int c, vector<Data> centroids, string metric)
{
	if(centroids.size() == 0)
	{
		return -1;
	}
	double dist, min;
	int res;
	if(c != 0)
	{
		min = distance(point.vector, centroids[0].vector, metric);
		res = 0;
	}
	else
	{
		min = distance(point.vector, centroids[1].vector, metric);
		res = 1;
	}
	for(int i = 1; i < centroids.size(); i++)
	{
		dist = distance(point.vector, centroids[i].vector, metric);
		if( (i != c) && ((dist > 0 && dist < min) || min == 0))
		{
			min = dist;
			res = i;
		}
	}
	if(res == c)
	{
		cout << "Error in second_best()";
		exit(EXIT_FAILURE);
	}
	return res;
}

double centroid_dist(Data point, vector<Data> centroids, string metric)
{
	if(centroids.size() == 0)
	{
		cout << "error in centroid_dist" << endl;
		exit(EXIT_FAILURE);
	}
	double min, dist;
	min = distance(point.vector, centroids[0].vector, metric);

	for(int i = 1; i < centroids.size(); i++)
	{
		dist = distance(point.vector, centroids[i].vector, metric);
		if(dist < min)
		{
			min = dist;
		}
	}
	return min;
}


void random_init(vector<Data> points, vector<Data>* centroids, int k)
{
	int temp[k] = {-1};												//picked centroids stored here
	int c, flag;
	random_device rd;
	mt19937 gen(rd());
	std::uniform_int_distribution<> int_distribution(0, points.size()-1);

	centroids->clear();												//make sure centroids is empty
	vector<Data>().swap(*centroids);
	for(int i = 0; i < k; i++)
	{
		do{															//ensure no doubles
			flag = 0;
			c = int_distribution(gen);
			for(int j = 0; j < i ; j++)
			{
				if(c == temp[j])
				{
					flag = 1;
					break;
				}
			}
		}while(flag);
		temp[i] = c;
		centroids->push_back(points[c]);
	}
}

void k_means_init(vector<Data> points, vector<Data>* centroids, int k, string metric)
{
	random_device rd;
	mt19937 gen(rd());
	srand(time(nullptr));

	int ctemp[k] = {-1};												//picked centroids stored here

	double p[points.size()+1];
	double random, max;
	int newcentroid, flag;
	Nearest_Centroid temp;

	centroids->clear();												//make sure centroids is empty
	vector<Data>().swap(*centroids);

	newcentroid = rand()%points.size();
	centroids->push_back(points[newcentroid]);
	ctemp[0] = newcentroid;
	for(int i = 1; i < k; i ++)
	{
		max = 0.0;
		p[0] = 0;
		for(int j = 1; j < points.size()+1; j++)
		{
			p[j] = centroid_dist(points[j-1], *centroids, metric);
			if(p[j] > max)
				max = p[j];
		}
		for(int j = 1; j < points.size()+1; j++)
		{
			if(max == 0)
			{
				cout << "k means++ init error" << endl;
				exit(EXIT_FAILURE);
			}
			p[j] = p[j-1] + (p[j]*p[j])/max;
			//cout << "p[" << j << "] = " << p[j] << endl;
		}
		std::uniform_real_distribution<> double_distribution(0, p[points.size()]-1);
		do
		{
			flag = 0;
			random = double_distribution(gen);//rand()%p[points.size()];						// rand in [0, p[j]-1] where j last probability
			newcentroid = binary_search(p, 0, points.size(), random);
			for(int j = 0; j < i; j++)
			{
				if(newcentroid == ctemp[j])
				{
					flag = 1;
					break;
				}
			}
		} while(flag);
		ctemp[i] = newcentroid;
		centroids->push_back(points[newcentroid]);
	}
}


void initialize(vector<Data> points, vector<Data>* centroids, int k, int algorithm, string metric)
{
	switch (algorithm){
	case 1:
		random_init(points, centroids, k);
		break;
	case 2:
		k_means_init(points, centroids, k, metric);
		break;
	default:
		cout << "No viable initialization function specified" << endl;
		exit(EXIT_FAILURE);
	}
}


void lloyd_assign(vector<Data>* points, vector<Data> centroids, string metric)
{
	Nearest_Centroid* temp;
	int flag;
	for(int i = 0; i < points->size(); i++)
	{
		temp = min_centroid(points->at(i), centroids, metric);
		if(temp != nullptr)
		{
			points->at(i).centroid = temp->centroid;
			points->at(i).dist = temp->dist;
			delete temp;
		}
	}
}




int lsh_replace(vector<Data>* points, vector<Neighbor>* range_results, int centroid, int loop)
{
	Data* point;
	double new_distance;
	int flag = 0;
	for(int i = 0; i < range_results->size(); i++)
	{
		point = range_results->at(i).data;
		new_distance = range_results->at(i).dist;
		if(point->centroid == -1)						//has not been set yet
		{
			point->centroid = centroid;
			point->dist = new_distance;
			point->range = loop;
			flag++;
		}
		else if(point->centroid != centroid && point->range == loop)		//was set in the same loop
		{
			if(point->dist > new_distance)						//distance of nearest centroid in Data larger than the one we just computed
			{	
				point->centroid = centroid;
				point->dist = new_distance;
				point->range = loop;
				flag++;			
			}
		}
	}
	return flag;
}

void reset_points(vector<Data>* points)
{
	for(int i = 0; i < points->size(); i++)	
	{
		points->at(i).centroid = -1;
		points->at(i).dist = -1;
		points->at(i).range = 0;
	}
}

double compute_range(vector<Data> centroids, int n, string metric)
{
	double dist, range = 0;
	for(int i = 0; i < centroids.size(); i++)
	{
		for(int j = i+1; j < centroids.size(); j++)
		{
			dist = distance(centroids[i].vector, centroids[j].vector, metric);
			if(dist < range || range == 0)
				range = dist;
		}
	}
	range = (double)range/n;
	return range;
}

void unassigned_points(vector<Data>* points, vector <Data> centroids, string metric)
{
	Nearest_Centroid* temp;
	int c;
	for(int i = 0; i < points->size(); i++)
	{
		if(points->at(i).range == 0)
		{
			temp = min_centroid(points->at(i), centroids, metric);
			points->at(i).centroid = temp->centroid;
			points->at(i).dist = temp->dist;
			delete temp;
		}
		else if((c = is_centroid(points->at(i), centroids)) >= 0)
		{
			points->at(i).centroid = c;
			points->at(i).dist = 0;
		}
	}
}

void lsh_assign(vector<Data>* points, vector<Data> centroids, HashTables* HT, int tables, string metric)
{
	double range = 0;
	int count, attempt = 0, loop = 1;
	vector<Neighbor>* range_results;


	reset_points(points);
	range = compute_range(centroids, 2, metric);

	while(attempt < 1)
	{
		count = 0;
		for(int i = 0; i < centroids.size(); i++)
		{
			range_results = RangeSearch(HT, centroids[i], tables, range, metric);
			count += lsh_replace(points, range_results, i, loop);
			range_results->clear();
			vector<Neighbor>().swap(*range_results);
			delete range_results;
		}
		if(!count)
			attempt++;
		range = range*2;
		loop = loop*2;
	}
	unassigned_points(points, centroids, metric);
/*	for(int i = 0; i < points->size(); i++)
	{
		cout << points->at(i).item << " " << points->at(i).centroid << " " << points->at(i).dist << " " << points->at(i).range << endl;
	}*/
}

void hypercube_assign(vector<Data>* points, vector<Data> centroids, HyperCube* cube, int hashfunctions, string metric)
{
	double range = 0;
	int count, attempt = 0, loop = 1;
	vector<Neighbor>* range_results;


	reset_points(points);
	range = compute_range(centroids, 2, metric);

	while(attempt < 1)
	{
		count = 0;
		for(int i = 0; i < centroids.size(); i++)
		{
			range_results = cube->range_search(centroids[i], hashfunctions, range);
			count += lsh_replace(points, range_results, i, loop);
			range_results->clear();
			vector<Neighbor>().swap(*range_results);
			delete range_results;
		}
		if(!count)
			attempt++;
		range = range*2;
		loop = loop*2;
	}
	unassigned_points(points, centroids, metric);
}

void assign(vector<Data>* points, vector<Data> centroids, HashTables* HT, HyperCube* cube, int tables, int hashfunctions, int algorithm, string metric)
{
	switch (algorithm){
	case 1:
		lloyd_assign(points, centroids, metric);
		break;
	case 2:
		lsh_assign(points, centroids, HT, tables, metric);
		break;
	case 3:
		hypercube_assign(points, centroids, cube, hashfunctions, metric);
		break;
	default:
		cout << "No viable assign function specified" << endl;
		exit(EXIT_FAILURE);
	}
}





int k_means_update(vector<Data> points, vector<Data>* centroids)
{
	int temp, pos, flag = 0;
	int count[centroids->size()] = {0};					//store how many points in each cluster
	vector<Data> prev = *centroids;						//copy old centroids
	for(int i = 0; i < centroids->size(); i++)			//wipe the old centroids
	{
		centroids->at(i).item = std::string("NN");
		for(int j = 0; j < centroids->at(i).vector.size(); j++)
		{
			centroids->at(i).vector[j] = 0.0;
		}
	}

	for(int i = 0; i < points.size(); i++)					//create new centroids
	{
		pos = points[i].centroid;
		count[pos]++;
		for(int j = 0; j < centroids->at(pos).vector.size(); j++)
		{
			centroids->at(pos).vector[j] += points[i].vector[j];
		}
	}
	for(int i = 0; i < centroids->size(); i++)
	{
		if(count[i] == 0)
			centroids->at(i).vector = prev[i].vector;
		else
		{
			for(int j = 0; j < centroids->at(i).vector.size(); j++)
			{
				centroids->at(i).vector[j] = centroids->at(i).vector[j]/(double)count[i];
			}
		}
		if(different(centroids->at(i).vector, prev[i].vector))					//even if one centroid has changed, set flag
		{
			flag = 1;
		}
	}
	return flag;
}

int pam_update(vector<Data> points, vector<Data>* centroids, string metric)
{
	vector<Data*> clusters[centroids->size()];
	double dist, min;
	int centroid, flag = 0;
	Data* new_centroid;
	for(int i = 0; i < points.size(); i++)
	{
		clusters[points[i].centroid].push_back(&points[i]);			//organize clusters by elements
	}
	
	for(int i = 0; i < centroids->size(); i++)
	{
		if(clusters[i].size() == 0)							//in pam every cluster should have at least the centroid
		{
			cout << "PAM " << centroids->at(i).item << "-" << i << " size 0" << endl;
			exit(EXIT_FAILURE);
		}
		double old = distance_sum(&(centroids->at(i)), clusters[i], metric);	//objective function of centroid
		min = distance_sum(clusters[i][0], clusters[i], metric);				//compute new 
		new_centroid = clusters[i][0];
		for(int j = 1; j < clusters[i].size(); j++)
		{
			dist = distance_sum(clusters[i][j], clusters[i], metric); 
			if(dist < min)
			{
				min = dist;
				new_centroid = clusters[i][j];
			}
		}
		if(min < old)
		{
			centroids->at(i).item = new_centroid->item;
			centroids->at(i).vector = new_centroid->vector;
			flag = 1;
		}
	}
	return flag;
}

int update(vector<Data> points, vector<Data>* centroids, int algorithm, string metric)
{
	switch (algorithm){
	case 1:
		return k_means_update(points, centroids);
	case 2:
		return pam_update(points, centroids, metric);
	default:
		cout << "No viable update function specified" << endl;
		exit(EXIT_FAILURE);
	}
}



double silhouette(vector<Data> points, vector<Data> centroids, int complete, string metric)
{
	vector<Data*> clusters[centroids.size()];
	int best, second;
	for(int i = 0; i < points.size(); i++)
	{
		clusters[points[i].centroid].push_back(&points[i]);
	}

	for(int i = 0 ; i < centroids.size(); i++)
	{
		cout << "CLUSTER-" << i << " {Size: " << clusters[i].size() << ", ";
		if(centroids[i].item == std::string("NN"))
		{
			for(int j = 0; j < centroids[i].vector.size(); j++)
			{
				cout << centroids[i].vector[j] << ", ";
			}
		}
		else
		{
			cout << centroids[i].item;
		}
		cout << "}" << endl;
		if(complete)
		{
			cout << "ITEMS {";
			for(int j = 0; j < clusters[i].size(); j++)
			{
				cout << clusters[i][j]->item << ", ";
			} 
			cout << "}" << endl;
		}
		cout << endl;
	}

	double a[points.size()] = {0};
	double b[points.size()] = {0};
	double s[points.size()] = {0};
	double silhouette = 0;	
	for(int i = 0; i < points.size(); i++)
	{
		best = points[i].centroid;
		second = second_best(points[i], best, centroids, metric);
		for(int j = 0; j < clusters[best].size(); j++)
		{
			a[i] += distance(points[i].vector, clusters[best][j]->vector, metric);
		}
		if(clusters[best].size() != 0){
			a[i] = (double)a[i]/clusters[best].size();
		}
		for(int j = 0; j < clusters[second].size(); j++)
		{
			b[i] += distance(points[i].vector, clusters[second][j]->vector, metric);
		}
		if(clusters[second].size() != 0){
			b[i] = (double)b[i]/clusters[second].size();
		}
		if(a[i] != 0 || b[i] != 0){
			s[i] = (double)(b[i] - a[i]) / max(a[i], b[i]);
		}
	}
	for(int i = 0; i < points.size(); i++)
	{
		silhouette += s[i];
	}
	silhouette = (double)silhouette/points.size();
	cout << "Silhouette: [" << silhouette << "]" << endl;
	return silhouette;
}

string algorithm_name(int init, int as, int upd)
{
	string alg = "";
	switch (init){
	case 1:
		alg += std::string("Random X ");
		break;
	case 2:
		alg += std::string("K-Means X ");
		break;
	default:
		break;
	}
	switch (as){
	case 1:
		alg += std::string("Lloyd X ");
		break;
	case 2:
		alg += std::string("LSH X ");
		break;
	case 3:
		alg += std::string("HyperCube X ");
		break;
	default:
		break;
	}
	switch (upd){
	case 1:
		alg += std::string("K-Means");
		break;
	case 2:
		alg += std::string("K-Medoids");
		break;
	default:
		break;
	}
	return alg;
}

void init_assign_update(vector<Data>* points, vector<Data>* centroids, int clusters, HashTables* hashtables, HyperCube* cube, int tables, int hashfunctions, int init, int as, int upd, int max, int complete, string metric)
{
	cout << "Algorithm: " << algorithm_name(init, as, upd) << endl;
	cout << "Metric: " << metric << endl;
	const clock_t begin_time = clock();
	initialize(*points, centroids, clusters, init, metric);
	//cout << "init " << endl;
	assign(points, *centroids, hashtables, cube, tables, hashfunctions, as, metric);
	//cout << "assign" << endl;
	int gg =0;

	while(update(*points, centroids, upd, metric))
	{
		gg++;
		assign(points, *centroids, hashtables, cube, tables, hashfunctions, as, metric);
		//cout << "assign" << endl;
		if(gg == max)
			break;
	}
	//cout << gg << endl;
	cout << "Clustering time: " << float(clock() - begin_time)/CLOCKS_PER_SEC << endl;
	silhouette(*points, *centroids, complete, metric);
}

void run_algorithms(vector<Data>* points, vector<Data>* centroids, int clusters, HashTables* hashtables, HyperCube* cube, int tables, int hashfunctions, int max, int complete, string metric)
{
	for(int i = 1; i <= 2; i++)
	{
		for(int j = 1; j <= 3; j++)
		{
			for(int k = 1; k <= 2; k++)
			{
				init_assign_update(points, centroids, clusters, hashtables, cube, tables, hashfunctions, i, j, k, max, complete, metric);
				cout << endl << endl;
			}
		}
	}
}