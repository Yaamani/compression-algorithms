#pragma once
#include "my_tree.h"
#include <vector>

int* dictionaryFreq(DictionaryTree&, int&);
long double* ranges(int*, int, int);

std::vector<int> generateDiffEncodedStream(std::list<int>&, int&, int&);
int* generateDiffFreq(std::vector<int>&, int, int, int&);

std::list<long long/*64bit*/> arithmitic_encoding(std::list<int>&, /*DictionaryTree&, int*,*/ int&);