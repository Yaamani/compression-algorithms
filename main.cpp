#include <string>
#include <iostream>
#include <list>
#include "my_utils.h"
#include "my_tree.h"
#include "lzw.h"
#include "arithmitic_coding.h"

int main()
{
    std::wstring enwik8 = readFileWideChars("enwik8");    


    DictionaryTree dictionary;
    std::list<int> encodedStream = 
            encodeAndSaveLZW(enwik8, dictionary, INT32_MAX, "enwik8 encoded");


    int dictionarySize;
    std::wstring decodedMessage = 
            loadAndDecodeLZW("enwik8 encoded", "enwik8 decoded", dictionarySize);

    return 0;
}