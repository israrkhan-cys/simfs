// fs_node.h - Just the FSNode structure
#ifndef FS_NODE_H
#define FS_NODE_H

#include <string>
#include <vector>
using namespace std;

struct FSNode {
    string name;                // File/directory name
    bool isDirectory;           // Type flag
    FSNode* parent;             // Points to parent (enables "..")
    vector<FSNode*> children;   // Unlimited children (N-ary)
    string content;             // File content (empty for dirs)
    string createdTime;

    FSNode(string n, bool isDir, FSNode* p);
    ~FSNode();
    FSNode* clone(FSNode* newParent);
};

#endif