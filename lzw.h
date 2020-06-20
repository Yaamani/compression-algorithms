#pragma once
#include "my_tree.h"
#include <string>
#include <vector>

std::list<int> encodeLZW(std::wstring&, DictionaryTree&, int);
std::wstring decodeLZW(std::list<int>&, int);

void saveEncodedLZW(std::string, std::list<int>&, int);
std::list<int> loadEncodedLZW(std::string, int&, int&);

std::list<int> encodeAndSaveLZW(std::wstring&, DictionaryTree&, int, std::string);
std::wstring loadAndDecodeLZW(std::string, std::string, int&);
