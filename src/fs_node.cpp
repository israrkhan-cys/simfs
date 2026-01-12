// fs_node.cpp - FSNode implementations
#include "../include/fs_node.h"
#include "../include/utils.h"
#include <algorithm>

using namespace std;

// Constructor
FSNode::FSNode(string n, bool isDir, FSNode* p) 
    : name(n), isDirectory(isDir), parent(p), content("") {
    createdTime = getCurrentTime();
}

// Destructor
FSNode::~FSNode() {
    for (FSNode* child : children) {
        delete child;
    }
    children.clear();
}

// Deep Copy
FSNode* FSNode::clone(FSNode* newParent) {
    FSNode* newNode = new FSNode(name, isDirectory, newParent);
    newNode->content = content;
    newNode->createdTime = getCurrentTime();
    
    for (FSNode* child : children) {
        newNode->children.push_back(child->clone(newNode));
    }
    return newNode;
}