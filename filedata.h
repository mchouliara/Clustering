#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>   //istringstream
#include "lsh.h"

#ifndef FILEDATA_H
#define FILEDATA_H

void command_line(int argc, char* argv[], string* input, string* config, string* output, string* metric, int* complete);
void configuration(string config, int* c, int* hf, int* t, int* max);
void file_counter(string file, int* lines, int* words_per_line, string* type);
void file_Data(vector<Data>*, string);	//parse data from file and insert to (pre-made) data set

#endif