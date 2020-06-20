#include "my_tree.h"
#include <iostream>

Node::Node(DictionaryTree* _base, wchar_t _c) 
{
    base = _base;
    c = _c;
    freq = 0;
}

Node* Node::addChild(wchar_t _c) 
{
    Node* node = new Node(base, _c);
    node->index = base->size;
    base->size++;
    children.push_back(node);
    return node;
}

Node* Node::searchInChildren(wchar_t _c) 
{
    for (Node* node : children)
        if (node->c == _c)
            return node;
    return nullptr;
}

std::list<Node*>& Node::getChildren() {
    return children;
}




DictionaryTree::DictionaryTree() {
    size = INITIAL_DICTIONARY_SIZE/*256*/;
}

Node* DictionaryTree::addChild(wchar_t _c) 
{
    Node* node = new Node(this, _c);
    children.push_back(node);
    /*if (_c > INITIAL_DICTIONARY_SIZE)
        std::cout << "           " << (int) _c << ' ';*/
    node->index = _c;
    return node;
}

Node* DictionaryTree::searchInChildren(wchar_t _c) 
{
    for (Node* node : children)
        if (node->c == _c)
            return node;
    return nullptr;
}

std::list<Node*>& DictionaryTree::getChildren() {
    return children;
}



// Node::Node(Tree* _base, wchar_t _c) 
// {
//     base = _base;
//     firstChild = nullptr;
//     nextSibling = nullptr;
//     c = _c;
//     index = base->size;
//     base->size++;
//     freq = 1;
// }

// Node* Node::setFirstChild (wchar_t _c) 
// {
//     Node* node = new Node(base, _c);
//     firstChild = node;
//     return node;
// }

// Node* Node::setNextSibling (wchar_t _c) 
// {
//     Node* node = new Node(base, _c);
//     nextSibling = node;
//     return node;
// }

// Node* Node::search(std::wstring_view _s) 
// {
//     if (_s[0] == c) 
//     {
//         if (_s.size() == 1) return this;
//         if (firstChild) return firstChild->search(_s.substr(1, _s.size())); 
//         return nullptr;
//     }
//     else 
//     {
//         if (nextSibling) return nextSibling->search(_s.substr(1, _s.size()));
//         return nullptr;
//     }
// }

// Node* Node::search(wchar_t _c) 
// {
//     if (_c == c) return this;
//     if (nextSibling) return nextSibling->search(_c);
//     return nullptr;
// }

// Tree::Tree (wchar_t _c) 
// {
//     Node* child = new Node(this, _c);
//     firstChild = child;
// }

// Node* Tree::search(std::wstring_view _s) 
// {
//     return firstChild->search(_s);
// }

// Node* Tree::search(wchar_t _c) 
// {
//     return firstChild->search(_c);
// }