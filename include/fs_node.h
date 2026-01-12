// fs_node.h - Just the FSNode structure
#ifndef FS_NODE_H
#define FS_NODE_H

#include <string>
#include <vector>
using namespace std;

struct FSNode {
    string name;
    bool isDirectory;
    FSNode* parent;
    vector<FSNode*> children;
    string content; 
    string createdTime;

    FSNode(string n, bool isDir, FSNode* p);
    ~FSNode();
    FSNode* clone(FSNode* newParent);
};

#endif