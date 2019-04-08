#include "filedata.h"
using namespace std;

void command_line(int argc, char* argv[], string* input, string* config, string* output, string* metric, int* complete)
{
	if(argc < 7)	//minimum required is input, output and config
	{
		cout << "./cluster –i  <input  file>  –c  <configuration  file>  -o  <output  file>  -d  <metric> " << endl;
		exit(EXIT_FAILURE);
	}
	*complete = 0;

	for(int i = 1; i < argc; i++)
	{
		if(argv[i] == std::string("-d"))
		{
			*metric = argv[i+1];
		}
		else if(argv[i] == std::string("-i"))
		{
			*input = argv[i+1];
		}
		else if(argv[i] == std::string("-c"))
		{
			*config = argv[i+1];
		}
		else if(argv[i] == std::string("-o"))
		{
			*output = argv[i+1];
		}
		else if(argv[i] == std::string("-complete"))
		{
			*complete = 1;
		}
	}
}

void configuration(string config_file, int* clusters, int* hashfunctions, int* tables, int* max)
{
	string line;
	string word;
	*clusters = 0;
	*hashfunctions = 0;
	*tables = 0;
	ifstream file(config_file);
	if (file.is_open())
	{
		while(getline(file, line, '\n'))
		{
			istringstream buf(line);
			buf >> word;
			if(word == std::string("number_of_clusters:"))
			{
				buf >> word;
				*clusters = stoi(word);
			}
			else if(word == std::string("number_of_hash_functions:"))
			{
				buf >> word;
				*hashfunctions = stoi(word);
			}
			else if(word == std::string("number_of_hash_tables:"))
			{
				buf >> word;
				*tables = stoi(word);
			}
			else if(word == std::string("max_loops:"))
			{
				buf >> word;
				*max = stoi(word);
			}
		}
		file.close();	
	}
	if(*clusters == 0)
		*clusters = 3;
	if(*hashfunctions == 0)
		*hashfunctions = 4;
	if(*tables == 0)
		*tables = 5;
}

void file_counter(string filename, int* lines, int* words_per_line, string* type)
{
	ifstream file (filename);
	string line, word;
	int l = 0, words = 0;
	if (file.is_open())
	{
		getline(file, line, '\n');			//first, get num of points in vector and type of LSH
		if(line == std::string("cosine") || line == std::string("euclidean"))
		{
			*type = line;
			getline(file, line, '\n');
		}
		else
		{
			*type = std::string("euclidean");
			l++;
		}
		istringstream buf(line);
		while(buf.good())
		{
			getline(buf, word, ',');
		    words++;
		}
		while(getline(file, line, '\n'))	//then get num of lines
		{
			l++;
		}
		file.close();
	}
	*words_per_line = words;
	*lines = l;
}

void file_Data(vector<Data>* dataset, string filename)
{
	int i = 0;
	point pvec;
	string line, x, item_name;
	Data dtemp;
	ifstream file (filename);
	if (file.is_open())
	{
		while(getline(file, line, '\n'))
		{
	    	istringstream buf(line);
	    	getline(buf, item_name, ',');
	    	while(buf.good())
	    	{
	    		getline(buf, x, ',');
	        	pvec.push_back(stod(x));
	    	}
	   		dtemp.set(item_name, pvec);
	   		dataset->push_back(dtemp);
	   		pvec.clear();
	   		i++;
		}
		file.close();
	}
}