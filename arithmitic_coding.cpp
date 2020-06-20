#include "arithmitic_coding.h"
#include "my_utils.h"
#include <list>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>

#define TWO_POWER_16 65536
#define TWO_POWER_MINUS_16 1.52587890625e-05
#define TWO_POWER_24 1.67772e+07
#define TWO_POWER_MINUS_24 5.96046e-08
#define TWO_POWER_32 4.29497e+09
#define TWO_POWER_MINUS_32 2.32831e-10
#define TWO_POWER_60 1.15292e+18
#define TWO_POWER_MINUS_60 8.67362e-19
#define TWO_POWER_53 9.0072e+15
#define TWO_POWER_MINUS_53 1.11022e-16

#define BINARY_32_ONES 4294967295
#define BINARY_1_SHIFTED_LEFT_32_TIMES 4294967296l /*(1<<32)*/
#define BINARY_31_ONES 2147483647

void getFreq(std::list<Node *> nodes, int *dictFreq, int &maxFreq)
{
    for (Node *child : nodes)
    {
        int currentFreq = child->freq;
        dictFreq[child->index] = currentFreq;

        if (currentFreq > maxFreq)
            maxFreq = currentFreq;

        if (!child->getChildren().empty())
            getFreq(child->getChildren(), dictFreq, maxFreq);
    }
}

int *dictionaryFreq(DictionaryTree &dictionary, int &maxFreq)
{
    int *dictFreq = new int[dictionary.size];
    for (int i = 0; i < dictionary.size; i++)
    {
        dictFreq[i] = 0;
    }
    maxFreq = 0;
    getFreq(dictionary.getChildren(), dictFreq, maxFreq);

    /*int totFreq = 0;
    for (int i = 0; i < dictionary.size; i++) {
        totFreq += dictFreq[i];
    }
    
    std::cout << totFreq << '\n';*/
    return dictFreq;
}

long double *ranges(int* freq, int freqSize, int encodedStreamSize)
{
    long double *dictRanges = new long double[freqSize];

    dictRanges[0] = (long double)freq[0] / encodedStreamSize;
    for (int i = 1; i < freqSize; i++)
    {
        dictRanges[i] = dictRanges[i - 1] + (long double)freq[i] / encodedStreamSize;
    }

    return dictRanges;
}

std::vector<int> generateDiffEncodedStream(std::list<int> &encodedStream, int &min, int &max)
{
    std::vector<int> diffEncodedStream;

    int j = 0;
    int iBefore = encodedStream.front();
    min = encodedStream.front();
    max = min;
    for (int &i : encodedStream)
    {
        if (j == 0)
        {
            diffEncodedStream.push_back(i);
            j++;
            continue;
        }

        int diff = i - iBefore;
        diffEncodedStream.push_back(diff);

        if (diff < min)
            min = diff;
        else if (diff > max)
            max = diff;

        iBefore = i;
        j++;
    }

    return diffEncodedStream;
}

int *generateDiffFreq(std::vector<int> &diffEncodedStream, int min, int max, int& arraySize)
{
    arraySize = max - min;
    int *diffFreq = new int[arraySize];
    for (int i = 0; i < arraySize; i++)
    {
        diffFreq[i] = 0;
    }

    //int actualSize = 0;
    for (int& i : diffEncodedStream)
    {
        diffFreq[i - min] += 1;

    }

    return diffFreq;
}

void add_to_buffer(bool bit,
                   int num,
                   long long &buffer,
                   int &buffer_size,
                   std::list<long long> &encoded_message)
{
    if (num <= 0)
        return;

    if (buffer_size + num >= 64)
    {
        int current_num = 64 - buffer_size;
        if (bit)
            // print('adding', current_num, 'ones to buffer')
            buffer = (buffer << current_num) ^ ((1 << current_num) - 1);
        else
            // print('adding', current_num, 'zeros to buffer')
            buffer = (buffer << current_num);

        encoded_message.push_back(buffer);
        buffer_size = 0;
        buffer = 0;

        add_to_buffer(bit, num - current_num, buffer, buffer_size, encoded_message);
    }
    else
    {
        // print('bit', bit)
        if (bit)
            // print('adding', num, 'ones to buffer')
            buffer = (buffer << num) ^ ((1 << num) - 1);
        else
            // print('adding', num, 'zeros to buffer')
            buffer = (buffer << num);
        // print('buffer_size before', buffer_size)
        buffer_size = buffer_size + num;
        // print('buffer_size after', buffer_size)
    }
}

bool encode_bit(long long &current_low_bin,
                long long &current_high_bin,
                long long &buffer,
                int &buffer_size,
                std::list<long long> &encoded_message)
{
    // shift Low to the left once
    // print('current_low_bin before shift\t\t', format(current_low_bin, '033b'))
    current_low_bin = current_low_bin << 1;
    // print('current_low_bin after shift\t\t', format(current_low_bin, '033b'))

    // shift High to the left once but the LSB = 1
    // print('current_high_bin before shift\t\t', format(current_high_bin, '033b'))
    current_high_bin = (current_high_bin << 1) ^ 1;
    // print('current_high_bin after shift\t\t', format(current_high_bin, '033b'))

    // select the 33th bit
    bool encoded_bit = (bool)(BINARY_1_SHIFTED_LEFT_32_TIMES & current_low_bin);
    // buffer_size = buffer_size+1
    if (encoded_bit)
    {
        // print('1 to encoded stream')
        // shift buffer to the left once but the LSB = 1
        add_to_buffer(encoded_bit, 1, buffer, buffer_size, encoded_message);
        // remove the 33th bit from Low & High
        current_low_bin = current_low_bin & (BINARY_32_ONES);
        // print('current_low_bin after removing 33th bit\t', format(current_low_bin, '032b'))
        current_high_bin = current_high_bin & (BINARY_32_ONES);
        // print('current_high_bin after removing 33th bit', format(current_high_bin, '032b'))
    }
    else
    {
        // print('0 to encoded stream')
        add_to_buffer(encoded_bit, 1, buffer, buffer_size, encoded_message);
    }
    /*if buffer_size >= 64:
        encoded_message = encoded_message + buffer*/
    return encoded_bit;
}

void get_low_high_bin(long double current_low,
                      long double current_high,
                      long long &current_low_bin,
                      long long &current_high_bin)
{
    // *TWO_POWER_32 to shift left 32 times in binary
    current_low_bin = current_low * TWO_POWER_32;
    if (current_high >= 1)
    {
        current_high_bin = current_high * TWO_POWER_32 - 1;
    }
    else
    {
        current_high_bin = current_high * TWO_POWER_32;
    }
}

void squeeze_out_2nd_MSB(long long &current_low_bin, long long &current_high_bin)
{
    // std::cout << ('current_low_bin before squeezing out the 31th bit\t', format(current_low_bin, '032b'))
    // std::cout << ('current_high_bin before squeezing out the 31th bit\t', format(current_high_bin, '032b'))

    // shift Low left then make the 32th bit 0 again
    current_low_bin = (current_low_bin << 1) & BINARY_31_ONES;

    // shift High left (LSB = 1) then make the 32th bit 1 again
    current_high_bin = (((current_high_bin << 1) & (BINARY_32_ONES)) | (1 << 31)) ^ 1;

    // std::cout << ('current_low_bin after squeezing out the 31th bit\t', format(current_low_bin, '032b'))
    // std::cout << ('current_high_bin before squeezing out the 31th bit\t', format(current_high_bin, '032b'))

    // std::cout << ()
}

std::list<long long /*64bit*/> arithmitic_encoding(std::list<int> &message, /*DictionaryTree& dictionary, int *dictFreq,*/ int &lastBufferSize)
{

    std::cout << "Arithmitic encoding....." << '\n';

    std::list<long long> encoded_message;
    long long buffer = 0;
    int buffer_size = 0;
    int underflow_counter = 0;

    std::cout << "Calculating diff....." << '\n';

    int min, max;
    std::vector<int> diffEncodedStream = generateDiffEncodedStream(message, min, max);

    std::cout << "Generating diff freq....." << '\n';

    int diffFreqSize;
    int* diffFreq = generateDiffFreq(diffEncodedStream, min, max, diffFreqSize);


    /*std::cout << "Generating frequencies....." << '\n';
    int maxFreq;
    dictFreq = dictionaryFreq(dictionary, maxFreq);*/

    std::cout << "Generating diff ranges....." << '\n';
    long double* diffRanges = ranges(diffFreq, diffFreqSize, diffEncodedStream.size());

    std::cout << "Encoding....." << '\n';

    long double current_low = 0;
    long double current_high = 1.0;

    int j = 0;
    // std::cout << (message);
    for (int &i : message)
    {
        // std::cout << (i)
        long double cl = current_low;
        long double d = current_high - cl;
        if (i > 0)
            current_low = cl + d * diffRanges[i - 1];
        else
            current_low = 0;
        current_high = cl + d * diffRanges[i];

        // std::cout << ('current low\t', current_low)
        // std::cout << ('current high\t', current_high)

        long long current_low_bin, current_high_bin;
        get_low_high_bin(current_low, current_high, current_low_bin, current_high_bin);

        // std::cout << ('current low\t', format(int(current_low_bin), '032b'))
        // std::cout << ('current high\t', format(int(current_high_bin), '032b'))

        // MSBs equality
        // std::cout << ('eq cond\t\t', current_low_bin >> 31, current_high_bin >> 31)
        if (current_low_bin >> 31 == current_high_bin >> 31)
        {
            bool encoded_bit = encode_bit(current_low_bin, current_high_bin, buffer, buffer_size, encoded_message);
            if (underflow_counter > 0)
            {
                // std::cout <<  (encoded_bit)
                if (encoded_bit)
                {
                    add_to_buffer(0, underflow_counter, buffer, buffer_size, encoded_message);
                    // std::cout << ('0'*underflow_counter, 'to encoded stream (Underflow)')
                }
                else
                {
                    add_to_buffer(1, underflow_counter, buffer, buffer_size, encoded_message);
                    // std::cout << ('1'*underflow_counter, 'to encoded stream (Underflow)')
                }
                // std::cout << ()
                // buffer_size = buffer_size+underflow_counter
                underflow_counter = 0;
            }
            // MSBs equality
            while (current_low_bin >> 31 == current_high_bin >> 31)
            {
                // std::cout << ('eq cond\t\t', current_low_bin >> 31, current_high_bin >> 31)
                encoded_bit = encode_bit(current_low_bin, current_high_bin, buffer, buffer_size, encoded_message);
            }
        }
        else if ((current_low_bin >> 30 ^ current_high_bin >> 30) == 0b11 and current_low_bin >> 30 == 1)
        {
            // Underflow
            // 31th bit equality

            // std::cout << ('Underflow')
            underflow_counter = underflow_counter + 1;
            squeeze_out_2nd_MSB(current_low_bin, current_high_bin);

            while (((current_low_bin >> 30 ^ current_high_bin >> 30) == 0b11) & (current_low_bin >> 30 == 1))
            {
                underflow_counter = underflow_counter + 1;
                squeeze_out_2nd_MSB(current_low_bin, current_high_bin);
            }
        }

        current_low = current_low_bin * TWO_POWER_MINUS_32;
        if (current_high_bin >= 0b11111111111111111111111111111111)
        {
            current_high = 1.0;
        }
        else
        {
            current_high = (current_high_bin)*TWO_POWER_MINUS_32;
        }

        /*std::cout << ('next low\t', current_low)
        std::cout << ('next high\t', current_high)
        std::cout << ('next low\t', format(current_low_bin, '032b'))
        std::cout << ('next high\t', format(current_high_bin, '032b'))
        std::cout << ('buffer', format(buffer, '0' + str(buffer_size) + 'b'), 'size', buffer_size)
        std::cout << ('============================================================================================')
        std::cout << ()*/
        j++;
        if (j % 100000 == 0)
            PrintProgressBar((float)j / message.size());
    }

    // final bits
    // current_low_bin, current_high_bin = get_low_high_bin(current_low, current_high)
    add_to_buffer(1, 1, buffer, buffer_size, encoded_message);
    // std::cout << ('last_buffer_size', buffer_size)
    /*current_low_bin = current_low_bin << 1
    current_high_bin = current_high_bin << 1
    while ((current_low_bin >> 31) ^ (current_high_bin >> 31)) & 1:
        buffer, buffer_size, encoded_message = add_to_buffer(1, 1, buffer, buffer_size, encoded_message)
        current_low_bin = current_low_bin << 1
        current_high_bin = current_high_bin << 1*/

    if (buffer_size > 0)
        encoded_message.push_back(buffer);

    PrintProgressBar(1);

    lastBufferSize = buffer_size;
    return encoded_message;
}