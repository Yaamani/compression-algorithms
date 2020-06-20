#include "lzw.h"
#include "my_tree.h"
#include "my_utils.h"
#include <list>
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <bitset>
#include <codecvt>
#include <iomanip>



std::list<int> encodeLZW(std::wstring& message, DictionaryTree& dictionary, int dictionarySizeLimit) {
    long messageSize = message.size();

    std::cout << "Encoding LZW....." << '\n';

    dictionary = DictionaryTree();
    std::list<int> encodedStream;
    
    long currentSeqPtr = 0;
    long lookAheadPtr = 0;

    while (lookAheadPtr < messageSize) {
        Node* currentNode = nullptr;
        Node* nextNode = dictionary.searchInChildren(message[currentSeqPtr]);

        while (nextNode)
        {
            lookAheadPtr++;
            currentNode = nextNode;
            if (lookAheadPtr >= messageSize) break;
            nextNode = currentNode->searchInChildren(message[lookAheadPtr]);
        }
        
        if (!currentNode) 
        {
            lookAheadPtr++;
            Node* alphabetChar = dictionary.addChild(message[currentSeqPtr]);
            
            alphabetChar->freq++;
            encodedStream.push_back(alphabetChar->index);
            
            if (dictionary.size < dictionarySizeLimit)
                alphabetChar->addChild(message[lookAheadPtr]);
        } 
        else 
        {
            if (dictionary.size < dictionarySizeLimit) {
                currentNode->addChild(message[lookAheadPtr]);
            }
            
            currentNode->freq++;
            encodedStream.push_back(currentNode->index);
        }

        currentSeqPtr = lookAheadPtr;

        if (lookAheadPtr % 100000 == 0)
            PrintProgressBar((float) lookAheadPtr/messageSize);
    }

    PrintProgressBar(1);
    std::cout << std::endl;
    // encodedStream.push_back(65536); // end of text
    return encodedStream;
}

std::wstring decodeLZW(std::list<int>& encodedStream, int dictionarySize) {
    std::wstring decodedMessage;

    std::cout << "Decoding....." << '\n';
    time_t start, finish;
    std::time(&start);

    wchar_t** dictionary = new wchar_t*[dictionarySize]; // Trading memory for performance (Classic)
    int* sizes = new int[dictionarySize];
    for (int currentEncodedSeq = 0; currentEncodedSeq < INITIAL_DICTIONARY_SIZE; currentEncodedSeq++) {
        sizes[currentEncodedSeq] = 1;
        dictionary[currentEncodedSeq] = new wchar_t;
        *dictionary[currentEncodedSeq] = currentEncodedSeq;
    }


    int recentDictionaryEntryIndex = INITIAL_DICTIONARY_SIZE;


    int firstChar = encodedStream.front();
    decodedMessage.append(dictionary[firstChar], sizes[firstChar]);
    dictionary[recentDictionaryEntryIndex] = new wchar_t[2];
    sizes[recentDictionaryEntryIndex] = 2;
    dictionary[recentDictionaryEntryIndex][0] = dictionary[firstChar][0];
    encodedStream.pop_front();    

    wchar_t* recentDictionaryEntry = dictionary[recentDictionaryEntryIndex];
    int recentDictionaryEntrySize = sizes[recentDictionaryEntryIndex];

    int i = 0;
    for (int& currentEncodedSeq : encodedStream) {
        wchar_t* currentDecodedSeqPtr = dictionary[currentEncodedSeq];
        int currentDecodedSeqSize = sizes[currentEncodedSeq];
        
        if (currentEncodedSeq == recentDictionaryEntryIndex) {
            recentDictionaryEntry[recentDictionaryEntrySize-1] = currentDecodedSeqPtr[0];
            // currentDecodedSeqSize++;
            decodedMessage.append(currentDecodedSeqPtr, currentDecodedSeqSize);
        } else {
            decodedMessage.append(currentDecodedSeqPtr, currentDecodedSeqSize);
            recentDictionaryEntry[recentDictionaryEntrySize-1] = currentDecodedSeqPtr[0];
        }

        if (recentDictionaryEntryIndex+1 <= dictionarySize) {
            int newDictionaryEntrySize = currentDecodedSeqSize+1;

            recentDictionaryEntryIndex++;
            dictionary[recentDictionaryEntryIndex] = new wchar_t[newDictionaryEntrySize];
            sizes[recentDictionaryEntryIndex] = newDictionaryEntrySize;
            recentDictionaryEntry = dictionary[recentDictionaryEntryIndex];
            recentDictionaryEntrySize = newDictionaryEntrySize;
            for (int currentEncodedSeq = 0; currentEncodedSeq < currentDecodedSeqSize; currentEncodedSeq++) {
                recentDictionaryEntry[currentEncodedSeq] = currentDecodedSeqPtr[currentEncodedSeq];
            }
        }

        i++;
        if (i % 100000 == 0)
            PrintProgressBar((float) i/encodedStream.size());
    }

    PrintProgressBar(1);
    std::cout << std::endl;

    std::time(&finish);
    std::cout << "Done in " << std::difftime(finish, start) << " seconds!!" << "\n\n";

    return decodedMessage;
}

void saveEncodedLZW(std::string fileName, std::list<int>& encodedStream, int dictionarySize) {
    int singleLiteralBits = std::ceil(std::log2(dictionarySize));

    std::cout << "Converting to bytes....." << '\n';

    std::string encodedBytes;

    char currentByte;
    int currentByteSize = 8;
    int currentLiteralSize = singleLiteralBits;

    int count = 0;
    for (int& i : encodedStream) {
        

        if (currentByteSize < 8) {
            currentLiteralSize = singleLiteralBits - (8-currentByteSize);
            currentByte |= (i >> (currentLiteralSize));
            currentByteSize = 8;
            encodedBytes += currentByte;

            // std::cout << std::bitset<19>(i) << '\n';

            /*for (int j = 0; j < singleLiteralBits - (currentLiteralSize+16); j++)
                std::cout << ' ';
            std::cout << std::bitset<8>(currentByte) << '\n';*/
        }

        // std::cout << '\n' << '\n' << std::bitset<19>(i) << '\n';

        if (currentLiteralSize == 0)
            currentLiteralSize = singleLiteralBits;

        while (currentLiteralSize > 0) {
            if (currentLiteralSize >= 8) {
                currentLiteralSize -= 8;
                currentByte = (char)(i >> currentLiteralSize);
                
                
                currentByteSize = 8;
                encodedBytes += currentByte;
                
                /*for (int j = 0; j < singleLiteralBits - (currentLiteralSize+8); j++)
                    std::cout << ' ';
                std::cout << std::bitset<8>(currentByte) << '\n';*/

            } else if (currentLiteralSize > 0) {
                currentByte = i << (8 - currentLiteralSize);
                
                // std::cout << std::bitset<19>((i >> currentLiteralSize)) << '\n';
                
                currentByteSize = currentLiteralSize;
                currentLiteralSize -= 8;

                /*for (int j = 0; j < singleLiteralBits - (currentLiteralSize+8); j++)
                    std::cout << ' ';
                std::cout << std::bitset<8>(currentByte) << '\n';*/
                
            }

        }

        count++;
        if (count % 100000 == 0)
            PrintProgressBar((float) count/encodedStream.size());  
    }

    if (currentByteSize < 8) {
        encodedBytes += currentByte;
    }


    PrintProgressBar(1);

    std::cout << '\n'/* << encodedBytes.size() << '\n'*/;

    std::cout << "Writing to disk....." << '\n';

    std::ofstream file;
    file.open(fileName, std::ios::binary);
    if (file.is_open()) {
        file << encodedBytes;
    }
    //std::cout << file.bad() << file.fail() << file.good() << '\n'; 
    file.close();

}

std::list<int> encodeAndSaveLZW(std::wstring& message, DictionaryTree& dictionary, int dictionarySizeLimit, std::string fileName) {
    time_t start, finish;
    std::time(&start);

    std::list<int> encodedStream = encodeLZW(message, dictionary, dictionarySizeLimit);

    // Just for simplicity
    std::ofstream dictionarySizeAndMessageSizeFile;
    dictionarySizeAndMessageSizeFile.open(fileName+"_dictionarySizeAndMessageSizeFile", std::ios::binary);
    if (dictionarySizeAndMessageSizeFile.is_open()) {
        dictionarySizeAndMessageSizeFile << message.size() << '\n' << dictionary.size;
    }
    dictionarySizeAndMessageSizeFile.close();

    saveEncodedLZW(fileName, encodedStream, dictionary.size);

    std::time(&finish);
    std::cout << "Done in " << std::difftime(finish, start) << " seconds!!" << "\n\n";

    return encodedStream;
}

std::list<int> loadEncodedLZW(std::string fileName, int& dictionarySize, int& messageSize) {
    std::list<int> encodedStream;

    time_t start, finish;
    std::time(&start);
    std::cout << "Reading the encoded file....." << '\n';

    std::ifstream dictionarySizeAndMessageSizeFile;
    dictionarySizeAndMessageSizeFile.open(fileName+"_dictionarySizeAndMessageSizeFile", std::ios::binary);
    if (dictionarySizeAndMessageSizeFile.is_open()) {
        dictionarySizeAndMessageSizeFile >> messageSize >> dictionarySize;
    }
    dictionarySizeAndMessageSizeFile.close();


    std::string encodedBytes = readFile2(fileName);
    /*std::ifstream file;
    file.open(fileName, std::ios::binary);
    if (file.is_open()) {
        file >> encodedBytes;
    }
    std::cout << file.bad() << file.fail() << file.good() << '\n';*/
    
    // std::cout << encodedBytes.size() << '\n';

    std::time(&finish);
    std::cout << "Done in " << std::difftime(finish, start) << " seconds!!" << "\n\n";


    std::cout << "Reconstructing the encoded stream....." << '\n';


    int singleLiteralBits = std::ceil(std::log2(dictionarySize));

    long currentLiteral = 0;
    int currentLiteralBitsLeft = singleLiteralBits;

    int i = 0; 
    for (char& c : encodedBytes) {

        // std::cout << std::bitset<19>(currentLiteral) << '\n';


        if (currentLiteralBitsLeft == 0) {
            encodedStream.push_back(currentLiteral);
            currentLiteral = 0;
            currentLiteralBitsLeft = singleLiteralBits;

            // std::cout << '\n';
        }



        int temp = 0;
        if (currentLiteralBitsLeft >= 8) {
            temp = c << (currentLiteralBitsLeft-8);
            currentLiteral |= temp & ((1 << currentLiteralBitsLeft)-1);
            currentLiteralBitsLeft -= 8;


            /*for (int j = 0; j < singleLiteralBits - (currentLiteralBitsLeft+8); j++)
                std::cout << ' ';
            std::cout << std::bitset<8>(c) << '\n';*/

        } else {
            int currentByteBitsLeft = 8-currentLiteralBitsLeft;
            temp = (c >> currentByteBitsLeft) & ((1 << currentLiteralBitsLeft)-1);
            currentLiteral |= temp;
            encodedStream.push_back(currentLiteral);

            // std::cout << std::bitset<19>(currentLiteral) << '\n';

            /*for (int j = 0; j < singleLiteralBits - (currentLiteralBitsLeft); j++)
                std::cout << ' ';
            std::cout << std::bitset<8>(c) << '\n';*/
            
            currentLiteral = 0;
            temp = (c & ((1 << currentByteBitsLeft)-1)) << (singleLiteralBits-currentByteBitsLeft);
            
            //std::cout << std::bitset<19>(temp) << '\n' << '\n';
            
            currentLiteralBitsLeft = singleLiteralBits - currentByteBitsLeft;
            currentLiteral |= temp;
            //encodedStream.push_back(currentLiteral);

            
        }

        i++;
        if (i % 100000 == 0)
            PrintProgressBar((float) i / encodedBytes.size());
    }

    if (currentLiteralBitsLeft == 0) {
            encodedStream.push_back(currentLiteral);
    }

    PrintProgressBar(1);

    std::cout << '\n'/* << encodedStream.size() << '\n'*/;


    return encodedStream;
}


std::wstring loadAndDecodeLZW(std::string inputFileName, std::string outputFileName, int& dictionarySize) {

    int /*dictionarySize, */messageSize;
    std::list<int> encodedStreamLoaded = loadEncodedLZW(inputFileName, dictionarySize, messageSize);

    std::wstring decodedMessage = decodeLZW(encodedStreamLoaded, dictionarySize);

    time_t start, finish;
    std::cout << "Writing the decoded file to disk....\n";
    std::time(&start);

    std::wstring_convert <std::codecvt_utf8_utf16 <wchar_t>> convert;
    std::ofstream myfile;
    myfile.open (outputFileName, std::ios::binary);
    myfile << convert.to_bytes(decodedMessage);
    myfile.close();
    
    std::time(&finish);
    std::cout << "Done in " << std::difftime(finish, start) << " seconds!!" << "\n\n";
    return decodedMessage;
}