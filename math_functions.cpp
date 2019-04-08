#include "math_functions.h"
using namespace std;

double multiply_vectors(point v1, point v2)
{
	if(v1.size() != v2.size())
	{
		return 0;
	}
	double d1, result = 0.0;
	for(int i = 0; i < v1.size(); i++)
	{
		d1 = v1[i]*v2[i];
		result += d1;
	}
	return result;
}

double euclidean_distance(point x, point y)
{
	double result = 0.0;
	if(x.size() != y.size())
		return -1;
	for(int i = 0; i < x.size(); i++)
	{
		result += (x[i] - y[i])*(x[i]-y[i]);
	}
	result = sqrt(result);
	//cout << "=" << endl << result << endl << endl;
	return result;
}

double cosine_distance(point x, point y)
{
	if(x.size() != y.size())
		return -1;
	double result;
	double x_y = multiply_vectors(x, y);
	double x_magn = 0.0, y_magn = 0.0;
	for(int i = 0; i < x.size(); i++)
	{
		x_magn += x[i]*x[i];
		y_magn += y[i]*y[i]; 
	}

	result = 1.0 - (x_y / (sqrt(x_magn) * sqrt(y_magn)));
	return result;
}

double distance(point x, point y, string type)
{
	if(type == std::string("euclidean"))
		return euclidean_distance(x,y);
	else
		return cosine_distance(x,y);
}

void hamming_distance(vector<int>* results, int point_length, int num, int maxdist, int cur)		//stin klhsh, cur = 1
{
	vector<int> r, n;
	int neighbor, temp = num;
	if(cur > maxdist)								//teleutaia anadromh
		return;

   	if(cur == 1)
   	{
   		(*results).push_back(num);
   	}
    while(temp != 0)
    {
		r.push_back(temp%2); //decimal to binary
    	temp = temp/2;
    }
    for(int i = r.size(); i < point_length; i++)				//i want the binary num to be d bytes long -> add extra 0s
    {
    	r.insert(r.end(), 0);
    }
    for(int i = 0; i < r.size(); i++)
    {
    	//cout << r[i] << " ";
    	n = r;
    	n[i] = -r[i] + 1;						//invert 1 bit
    	neighbor = 0;
    	for(int j = 0; j < n.size(); j++)				//turn to decimal and add to neighbors
    	{
    		neighbor += n[j]*pow(2,j);
    	}
    	hamming_distance(results, point_length, neighbor, maxdist, cur+1);
    	if(find((*results).begin(), (*results).end(), neighbor) == (*results).end())
    		(*results).push_back(neighbor);
    	n.clear();
    }
}

int different(point p1, point p2)
{
	if(p1.size() != p2.size())
		return 1;
	for(int i = 0; i < p1.size(); i++)
	{
		if(p1[i] != p2[i])
			return 1;
	}
	return 0;
}

int binary_search(double table[], int l, int r, double num) //https://www.geeksforgeeks.org/binary-search, modified
{
	int mid = l + (r - l)/2; 
	if (num >= table[mid-1] && num < table[mid])   
	    return mid;

	if (num < table[mid-1])  
	    return binary_search(table, l, mid-1, num); 

	return binary_search(table, mid+1, r, num); 
}

double distance_sum(Data* point, vector<Data*> all_points, string metric)
{
	double result = 0.0;
	for(int i = 0; i < all_points.size(); i++)
	{
		result += distance(point->vector, all_points[i]->vector, metric);
	}
	return result;
}