#include "my_utils.h"
#include <cstdio>
#include <fstream>
#include <vector>
#include <codecvt>
#include <locale>
#include <iostream>
#include <iomanip>
#include <locale>

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

// https://stackoverflow.com/a/36315819/3396222
void PrintProgressBar(float progress) {
    int val = (int) (progress * 100);
    int lpad = (int) (progress * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

// https://stackoverflow.com/a/116220/3396222
std::string readFile2(const std::string &fileName)
{
    std::ifstream ifs(fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);
    return std::string(bytes.data(), fileSize);
}

// https://stackoverflow.com/a/51356708/3396222
std::wstring widen(const std::string& utf8_string)
{
    std::wstring_convert <std::codecvt_utf8_utf16 <wchar_t>> convert;   
    return convert.from_bytes(utf8_string);
}

std::wstring readFileWideChars(const std::string &fileName) {
    time_t start, finish;
    
    std::cout << "Reading file....." << '\n';
    std::time(&start);

    std::wstring data = widen(readFile2(fileName));

    std::time(&finish);
    std::cout << "Done in " << std::difftime(finish, start) << " seconds!!" << "\n\n";

    return data;

}