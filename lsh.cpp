#include "lsh.h"
#include "math_functions.h"
using namespace std;

random_device rd;
mt19937 gen(rd());
std::normal_distribution<float> norm_distribution(0.0,1.0);
std::uniform_real_distribution<double> uni_distribution(0.0,W);
//std::uniform_int_distribution<> int_distribution(0, 1);


H1::H1(point newv, double newt)
{
	v = newv;
	t = newt;
}


H2::H2(int k)
{
	hash_data = new H1[k];
	r_vector = new int[k];
	hash_num = k;
}

void H2::set(int vector_size)
{
	double newt, v_i;
	point newv;
	for(int i = 0; i < hash_num; i++)
	{
		r_vector[i] = rand() - RAND_MAX/2;
		newt = uni_distribution(gen);
		for(int j = 0; j < vector_size; j++)
		{
		    newv.push_back(norm_distribution(gen));
		}
		H1 h(newv, newt);
		hash_data[i] = h;
		newv.clear();
	}
	uni_distribution.reset();
	norm_distribution.reset();
}


void H2::print()
{
	for(int i = 0; i < hash_num; i++)
	{
		cout << "Hash Function " << i << ":" << endl;
		cout << "t: " << hash_data[i].t << endl << "v: " ;
		for(int j = 0; j < hash_data[i].v.size(); j++)
		{
			cout << hash_data[i].v[j] << " " ;
		}
		cout << endl;
	}
}

int H2::memory()
{
	return (sizeof(H1)*hash_num + sizeof(int)*hash_num + sizeof(int));
}

H2::~H2()
{
	delete[] hash_data;
	delete[] r_vector;
}

////////////////////////////////////////////////

HashTable::HashTable(int b, int k, int d)
{
	table_size = b;
	hash_methods = new H2(k);
	hash_methods->set(d);
	buckets = new vector<Data*>[b];
}


void HashTable::print()
{
	Data* dtemp;
	point ptemp;
	long int sum = 0;
	for(int i = 0; i < table_size; i++)
	{
		cout << endl << "Bucket " << i << " has " << buckets[i].size() << " items" << endl;
		sum += buckets[i].size();
		for(int j = 0; j < buckets[i].size(); j++)
		{
			cout << "List item " << j << ": ";;
			dtemp = buckets[i][j];
			cout << "Name :" << dtemp->item << endl;
			//cout << "Vector: ";
			// ptemp = dtemp->vector;
			// for(int k = 0; k < ptemp.size(); k++)
			// {
			// 	cout << (ptemp)[k] << " ";
			// }
			// cout << endl;
		}
	}
	cout << "sum: " << sum << endl;
}

HashTable::~HashTable()
{
	delete hash_methods;
	for(int i = 0; i < table_size; i++)
	{
		buckets[i].clear();
		vector<Data*>().swap(buckets[i]);
	}
	delete[] buckets;
}

////////////////////////////////////////////////

void Euclidean_HT::euclidean(Data* d, string* key, int* position)
{
	int h[hash_methods->hash_num];
	int r;
	long long int res = 0;
	long int m = 34359738363; 	//2^32 - 5
	double p_v, t;
	point v, x = d->vector;
	*position = 0;
	string g = "";

	for(int i = 0; i < hash_methods->hash_num; i++)			//h[i] = (p*v + t)/w
	{
		v = hash_methods->hash_data[i].v;
		t = hash_methods->hash_data[i].t;
		p_v = multiply_vectors(x, v);
		h[i] = (p_v + t)/ W;
		g += to_string(h[i]);
		g += std::string("/");
	}
	for(int i = 0; i < hash_methods->hash_num; i++)		//method to compute final hash function phi
	{
		r = hash_methods->r_vector[i];
		if(h[i]*r >= 0)
		{
			res += (h[i]*r)%m;
		}
		else								//https://math.stackexchange.com/questions/519845/modulo-of-a-negative-number
		{
			res += ((-h[i]*r)*(m-1))%m;
		}
	}
	res = abs((res%m)%table_size);
	*key = g;
	*position = (int) res;	
}

void Euclidean_HT::insert(Data* d)
{
	string key = "";
	int position;
	this->euclidean(d, &key, &position);
	buckets[position].push_back(d);
	g_functions[position].push_back(key);
}

Neighbor* Euclidean_HT::nearest_neighbor(Data* query)
{
	Neighbor *n;
	double dist, min = -1;
	Data* dmin;
	string key = "";
	int position;
	this->euclidean(query, &key, &position);
	vector<Data *> bucket = buckets[position];

	if(bucket.size() == 0)
	{
		return nullptr;
	}

	for(int i = 0; i < bucket.size() ; i++)
	{
		if(key == g_functions[position][i])									//matching set of h[1]...h[k]
		{
			dist = euclidean_distance(query->vector, bucket[i]->vector);
			if ( (dist < min || min <= 0) && dist > 0)
			{
				min = dist;
				dmin = bucket[i];
			}
		}
	}
	if(min == -1)
		return nullptr;
	n = new Neighbor();
	n->dist = min;
	n->data = dmin;
	return n;
}

vector<Neighbor>*  Euclidean_HT::range_search(Data * query, double r)
{
	vector<Neighbor>* result = new vector<Neighbor>;
	Neighbor ntemp;
	double dist, max= r;
	Data* d;
	int position;
	string key;
	this->euclidean(query, &key, &position);
	vector<Data *> bucket = buckets[position];
	for(int i = 0; i < bucket.size() ; i++)
	{
		d = bucket[i];
		dist = euclidean_distance(query->vector, d->vector);
		if(dist <= r)
		{
			ntemp.dist = dist;
			ntemp.data = d;
			result->push_back(ntemp);
		}
	}
/*	for(int i = 0; i < result->size(); i++)
		cout << result->at(i).dist << endl;*/
	return result;
}

int Euclidean_HT::memory()
{
	long int sum = 0;
	for(int i = 0; i < table_size; i++)
		sum += buckets[i].size();
	return (sizeof(int) + hash_methods->memory() + sizeof(vector<Data>)*table_size + sizeof(Data)*sum + sizeof(vector<string>)*table_size + sizeof(string)*sum);
}

/////////////////////////////////////////////////////////////////////////////

int Cosine_HT::cosine(Data* d)
{
	int h[hash_methods->hash_num];
	int position = 0;
	double res;
	point v, x = d->vector;
	for(int i = 0; i < hash_methods->hash_num; i++)			//h[i] = 0 || 1
	{
		v = hash_methods->hash_data[i].v;
		res = multiply_vectors(x, v);
		if(res >= 0)
			h[i] = 1;
		else
			h[i] = 0;
	}
	for(int i = 0; i < hash_methods->hash_num; i++) 	//"binary" to decimal
	{
		position += h[i]*pow(2,i);
	}
	return position;
}

void Cosine_HT::insert(Data* d)
{
	int position = this->cosine(d);
	buckets[position].push_back(d);
}

Neighbor* Cosine_HT::nearest_neighbor(Data* query)
{
	Neighbor *n;
	double dist, min;
	Data* dmin;
	int position = this->cosine(query);
	vector<Data *> bucket = buckets[position];

	if(bucket.size() == 0)
	{
		return nullptr;
	}
	min = cosine_distance(query->vector, bucket[0]->vector);
	dmin = bucket[0];
	for(int i = 1; i < bucket.size() ; i++)
	{
		dist = cosine_distance(query->vector, bucket[i]->vector);
		if ( (dist < min || min == 0) && dist > 0)
		{
			min = dist;
			dmin = bucket[i];
		}
	}
	n = new Neighbor();
	n->dist = min;
	n->data = dmin;
	return n;
}


vector<Neighbor> *  Cosine_HT::range_search(Data * query, double r)
{
	vector<Neighbor> * result = new vector<Neighbor>;
	Neighbor ntemp;
	double dist, max= r;
	Data* d;
	int position = this->cosine(query);
	vector<Data *> bucket = buckets[position];
	for(int i = 0; i < bucket.size() ; i++)
	{
		d = bucket[i];
		dist = cosine_distance(query->vector, d->vector);
		if(dist <= r)
		{
			ntemp.dist = dist;
			ntemp.data = d;
			result->push_back(ntemp);
		}
	}
	return result;
}

int Cosine_HT::memory()
{
	long int sum = 0;
	for(int i = 0; i < table_size; i++)
		sum += buckets[i].size();
	return (sizeof(int) + hash_methods->memory() + sizeof(vector<Data>)*table_size + sizeof(Data)*sum);
}


////////////////////////

int HyperCube::find(int num)					//-1 if not found, number it translates to (0 or 1) if found
{
	for(int i= 0; i < 2; i++)
	{
		for(int j = 0; j < dictionary[i].size(); j++)
		{
			if(dictionary[i][j] == num)
			{
				return i;
			}
		}
	}
	return -1;
}

int HyperCube::cube_pos(Data* d)
{
	int pos = 0;
	int h;
	point v, x = d->vector;
	if(metric == std::string("cosine"))
	{
		double res;
		for(int i = 0; i < hash_methods->hash_num; i++)			//h[i] = 0 || 1
		{
			v = hash_methods->hash_data[i].v;
			res = multiply_vectors(x, v);
			if(res >= 0)
				h = 1;
			else
				h = 0;
			pos += h*pow(2,i);							//"binary" to decimal
		}
	}
	else
	{
		int f;
		double p_v, t;

		for(int i = 0; i < hash_methods->hash_num; i++)			//h[i] = |(p*v + t)/w|
		{
			v = hash_methods->hash_data[i].v;
			t = hash_methods->hash_data[i].t;
			p_v = multiply_vectors(x, v);
			h = (p_v + t)/ W;
			f = this->find(h);							//find in dictionary
			if(f == -1)
			{
				f = rand()%2;//int_distribution(generator);
				dictionary[f].push_back(h);
			}
			pos += f*pow(2, i);							//"binary" to decimal
		}
	}
	return pos;
}

void HyperCube::insert(Data* d)
{
	int pos = cube_pos(d);
	buckets[pos].push_back(d);
}

void HyperCube::insert_dataset(vector<Data>* points)
{
	for(int i = 0; i < points->size() ; i++)
	{
		this->insert(&(points->at(i)));
	}
}

double HyperCube::nearest_neighbor(Data *query, int n, float* time)
{
	int position = cube_pos(query);
	int k = 1, count = 0;						//counter of how many points we've examined
	double dist, min;
	string minitem;
	vector<int> bucket_no;
	vector<Data *> current;

	const clock_t begin_time = clock();

	do{
		bucket_no.clear();
		hamming_distance(&bucket_no, n, position, k, 1);		//all buckets i need to search are in here 
		k++;
	}while(bucket_no.size() < probes);							//hamilton distance k to have probes buckets available
	
	k = 0;
	do{
		current = buckets[bucket_no[k]];
		k++;
	}while(current.size() == 0);

	if(metric == std::string("cosine"))
		min = cosine_distance(query->vector, current[0]->vector);
	else
		min = euclidean_distance(query->vector, current[0]->vector);
	minitem = current[0]->item;
	for(int i = k; i < probes; i++)
	{
		current = buckets[bucket_no[i]];
		count = 0;
		for(int j = 0; j < current.size() ; j++)
		{
			if(metric == std::string("cosine"))
				dist = cosine_distance(query->vector, current[j]->vector);
			else
				dist = euclidean_distance(query->vector, current[j]->vector);
			if(dist > 0 && dist < min)
			{
				min = dist;
				minitem = current[j]->item;
			}
			count++;
			if(count >= maxneighbors)
				break;
		}
	}
	cout << "Nearest Neighbor: " << minitem << endl;
	cout << "distanceLSH: " << min << endl;
	*time = float(clock ()-begin_time )/CLOCKS_PER_SEC;
	cout << "tLSH: " << *time << endl;
	return min;
}

vector<Neighbor>* HyperCube::range_search(Data query, int point_length, double r)
{
	vector<Neighbor>* result = new vector<Neighbor>;
	Neighbor ntemp;
	double dist, max= r;
	Data* d;
	int position = cube_pos(&query);
	int count = 0, k = 1;

	vector<int> bucket_no;

	do{
		bucket_no.clear();
		hamming_distance(&bucket_no, point_length, position, k, 1);		//all buckets i need to search are in here 
		k++;
	}while(bucket_no.size() < probes);

	vector<Data *> current;
	for(int i = 0; i < probes; i++)
	{
		current = buckets[bucket_no[i]];
		for(int j = 0; j < current.size() ; j++)
		{
			d = current[j];
			if(metric == std::string("cosine"))
				dist = cosine_distance(query.vector, d->vector);
			else
				dist = euclidean_distance(query.vector, d->vector);
			if(dist <= r)
			{
				ntemp.dist = dist;
				ntemp.data = d;
				result->push_back(ntemp);
			}
			count++;
			if(count >= maxneighbors)
				break;
		}
		if(count >= maxneighbors)
			break;
	}
	return result;
/*	cout << "R-Near Neighbors: " << endl;
	for(int i = 0; i < result.size(); i++)
	{
		cout << result[i].data.item << " " << result[i].dist << endl;
	}*/
}

int HyperCube::memory()
{
	long int sum = 0;
	for(int i = 0; i < table_size; i++)
		sum += buckets[i].size();
	return (sizeof(int) + hash_methods->memory() + sizeof(vector<Data>)*table_size + sizeof(Data)*sum + 2*sizeof(int) + sizeof(string) + 2*sizeof(vector<int>) + sizeof(int)*(dictionary[0].size() + dictionary[1].size()));
}

int is_duplicate(Neighbor n, vector<Neighbor> vn)
{
	int found = 0;
	for(int i = 0; i < vn.size(); i++)
	{
		if(vn[i].data->item == n.data->item)
		{
			found = 1;
			break;
		}
	}
	return found;
}

vector<Neighbor>* RangeSearch(HashTables* HT, Data query, int l, double r, string type)
{
	vector<Neighbor>* ntemp;
	vector<Neighbor>* neighbors = new vector<Neighbor>;
	if(type == std::string("euclidean"))
	{
		//Euclidean_HT** HashTables = HT->Euc_HT;
		for(int j = 0; j < l; j++)
		{
			ntemp = HT->Euc_HT[j]->range_search(&(query), r);
			if(ntemp->size() > 0)
			{
				for(int k = 0; k < ntemp->size(); k++)
				{
					if(!is_duplicate((*ntemp)[k], *neighbors))
					{
						neighbors->push_back((*ntemp)[k]);
					}
				}
			}
			ntemp->clear();
			delete ntemp;
		}
	}
	else
	{
		//Cosine_HT** HashTables = HT->Cos_HT;
		for(int j = 0; j < l; j++)
		{
			ntemp = HT->Cos_HT[j]->range_search(&(query), r);
			if(ntemp->size() > 0)
			{
				for(int k = 0; k < ntemp->size(); k++)
				{
					if(!is_duplicate((*ntemp)[k], *neighbors))
						neighbors->push_back((*ntemp)[k]);
				}
			}
			ntemp->clear();
			delete ntemp;
		}
	}
	return neighbors;
}