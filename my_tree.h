#pragma once
#include <string>
#include <list>

//#define INITIAL_DICTIONARY_SIZE 65536 // total number of all chars a 'wchar_t' can hold
#define INITIAL_DICTIONARY_SIZE 166416 // wchar_t on mac and linux is 32bits not 16bits, so there're some chars > 65536 on my labtop

struct DictionaryTree;

struct Node 
{
    DictionaryTree* base;
    std::list<Node*> children;
    wchar_t c;
    int index;
    int freq;

    Node(DictionaryTree*, wchar_t);
    Node* addChild(wchar_t);
    Node* searchInChildren(wchar_t);
    std::list<Node*>& getChildren();
};

struct DictionaryTree
{
    std::list<Node*> children;
    int size;
    
    DictionaryTree();
    Node* addChild(wchar_t);
    Node* searchInChildren(wchar_t);
    std::list<Node*>& getChildren();
};
