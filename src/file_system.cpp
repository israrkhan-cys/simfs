// file_system.cpp
#include "../include/file_system.h"
#include "../include/utils.h"
#include "../include/colors.h"

#include <iostream>
#include <stack>
#include <algorithm>
#include <cctype>     



using namespace std;

// ========== PRIVATE HELPERS ==========
FSNode* FileSystem::findChild(FSNode* parent, const string& name) {
    for (FSNode* child : parent->children) {
        if (child->name == name) {
            return child;
        }
    }
    return nullptr;
}

vector<string> FileSystem::split(const string& str, char delimiter) {
    return ::split(str, delimiter);  // Use the global split function
}

FSNode* FileSystem::resolvePath(const string& path) {
    if (path == "/") return root;
    
    FSNode* trav = (path.front() == '/') ? root : currentDir;
    vector<string> tokens = split(path, '/');

    for (const string& token : tokens) {
        if (token == ".") continue;
        else if (token == "..") {
            if (trav != root) trav = trav->parent;
        }
        else {
            FSNode* next = findChild(trav, token);
            if (next) trav = next;
            else return nullptr;
        }
    }
    return trav;
}

string FileSystem::getFullPath(FSNode* node) {
    if (node == root) return "/";
    stack<string> pathStack;
    FSNode* temp = node;
    while (temp != root) {
        pathStack.push(temp->name);
        temp = temp->parent;
    }
    string path = "";
    while (!pathStack.empty()) {
        path += "/" + pathStack.top();
        pathStack.pop();
    }
    return path;
}

// ========== CONSTRUCTOR/DESTRUCTOR ==========
FileSystem::FileSystem() {
    root = new FSNode("/", true, nullptr);
    root->parent = root;
    currentDir = root;
}

FileSystem::~FileSystem() {
    delete root;
}

// ========== PUBLIC METHODS ==========
string FileSystem::getPromptPath() {
    if (currentDir == root) return "/";
    return getFullPath(currentDir);
}


//mkdir implementation
void FileSystem::mkdir(const string& name) {
    if (findChild(currentDir, name)) {
        cout << Colors::RED << "Error:"<< Colors::RESET <<" Directory exists." << endl;
        return;
    }
    FSNode* newNode = new FSNode(name, true, currentDir);
    currentDir->children.push_back(newNode);
}

//Touch
void FileSystem::touch(const string& name) {
    if (findChild(currentDir, name)) return;
    FSNode* newNode = new FSNode(name, false, currentDir);
    currentDir->children.push_back(newNode);
}

//ls
void FileSystem::ls() {
    vector<FSNode*> displayList = currentDir->children;
    
    sort(displayList.begin(), displayList.end(), [](FSNode* a, FSNode* b) {
        return a->name < b->name;
    });

    for (FSNode* node : displayList) {
        if (node->isDirectory) 
            cout << Colors::BLUE << node->name << Colors::RESET << "/  ";
        else 
            cout << Colors::WHITE << node->name << Colors::RESET << "  ";
    }
    cout << endl;
}

//cd cammand
void FileSystem::cd(const string& path) {
    FSNode* target = resolvePath(path);
    if (target && target->isDirectory) {
        currentDir = target;
    } else if (target && !target->isDirectory) {
        cout << Colors::RED << "Error: Not a directory." << Colors::RESET << endl;
    } else {
        cout << Colors::RED << "Error: Directory not exist." << Colors::RESET << endl;
    }
}

//rm cammand 
void FileSystem::rm(const string& name) {
    auto it = currentDir->children.begin();
    while (it != currentDir->children.end()) {
        if ((*it)->name == name) {
            delete *it;
            currentDir->children.erase(it);
            return;
        }
        ++it;
    }
    cout << Colors::RED << "Error: File not found." << Colors::RESET << endl;
}

void FileSystem::writeToFile(const string& name, const string& text) {
    FSNode* node = findChild(currentDir, name);
    if (!node) {
        node = new FSNode(name, false, currentDir);
        currentDir->children.push_back(node);
    }
    
    if (node->isDirectory) {
        cout << Colors::RED << "Error: Cannot write to a directory." << Colors::RESET << endl;
        return;
    }
    node->content = text;
}

//cat cammand 
void FileSystem::cat(const string& name) {
    FSNode* node = findChild(currentDir, name);
    if (!node) {
        cout<< Colors::RED<< "Error: File not found." << Colors::RESET << endl;
    } else if (node->isDirectory) {
        cout<< Colors::RED<< "Error: Is a directory." << Colors::RESET << endl;
    } else {
        cout << node->content<< endl;
    }
}

void FileSystem::mv(const string& srcName, const string& destName) {
    auto it = currentDir->children.begin();
    FSNode* srcNode = nullptr;
    int srcIndex = -1;

    for (int i = 0; i < currentDir->children.size(); i++) {
        if (currentDir->children[i]->name == srcName) {
            srcNode = currentDir->children[i];
            srcIndex = i;
            break;
        }
    }

    if (!srcNode) {
        cout << Colors::RED << "Error: Source not found." << Colors::RESET << endl;
        return;
    }

    FSNode* possibleDestDir = resolvePath(destName);

    if (possibleDestDir && possibleDestDir->isDirectory) {
        currentDir->children.erase(currentDir->children.begin() + srcIndex);
        srcNode->parent = possibleDestDir;
        possibleDestDir->children.push_back(srcNode);
    } 
    else {
        if (findChild(currentDir, destName)) {
            cout << Colors::RED << "Error: Destination name already exists." << Colors::RESET << endl;
            return;
        }
        srcNode->name = destName;
    }
}

void FileSystem::cp(const string& srcName, const string& destName) {
    FSNode* srcNode = findChild(currentDir, srcName);
    if (!srcNode) {
        cout << Colors::RED << "Error: Source not found." << Colors::RESET << endl;
        return;
    }
    if (findChild(currentDir, destName)) {
        cout << Colors::RED << "Error: Destination exists." << Colors::RESET << endl;
        return;
    }

    FSNode* newNode = srcNode->clone(currentDir);
    newNode->name = destName;
    currentDir->children.push_back(newNode);
}


void FileSystem::pwd(){
    string pwdd = getFullPath(currentDir);
    cout<<pwdd<<endl;
}

void FileSystem::CreateSystem(){
   mkdir("Home");
   mkdir("Downloads");
   mkdir("Documents");
   mkdir("Desktop");
   mkdir("Pictures");
   mkdir("Music");
   mkdir("Vedios");
}



// Helper: Simple wildcard matching (supports * only) ...used by the find function to search for any files with  some type 
bool FileSystem::matchesPattern(const std::string& name, const std::string& pattern) {
    // If pattern is empty, match everything
    if (pattern.empty()) return true;
    
    // Simple exact match (case-insensitive)
    std::string nameLower = name;
    std::string patternLower = pattern;
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
    std::transform(patternLower.begin(), patternLower.end(), patternLower.begin(), ::tolower);
    
    // Check for wildcard *
    size_t wildcardPos = patternLower.find('*');
    
    if (wildcardPos == std::string::npos) {
        // No wildcard - exact match
        return nameLower == patternLower;
    } else if (patternLower == "*") {
        // Just * matches everything
        return true;
    } else if (wildcardPos == 0) {
        // Pattern starts with * (ends with)
        std::string suffix = patternLower.substr(1);
        if (nameLower.length() < suffix.length()) return false;
        return nameLower.substr(nameLower.length() - suffix.length()) == suffix;
    } else if (wildcardPos == patternLower.length() - 1) {
        // Pattern ends with * (starts with)
        std::string prefix = patternLower.substr(0, patternLower.length() - 1);
        if (nameLower.length() < prefix.length()) return false;
        return nameLower.substr(0, prefix.length()) == prefix;
    } else {
        // Wildcard in middle - split and check
        std::string prefix = patternLower.substr(0, wildcardPos);
        std::string suffix = patternLower.substr(wildcardPos + 1);
        
        if (nameLower.length() < prefix.length() + suffix.length()) return false;
        return nameLower.substr(0, prefix.length()) == prefix &&
               nameLower.substr(nameLower.length() - suffix.length()) == suffix;
    }
}

// Search in specific directory
std::vector<std::string> FileSystem::findInDirectory(FSNode* dir, 
                                                    const std::string& pattern,
                                                    const std::string& currentPath,
                                                    bool searchFiles,
                                                    bool searchDirs) {
    std::vector<std::string> results;
    
    if (!dir || !dir->isDirectory) return results;
    
    // Search in current directory
    for (FSNode* child : dir->children) {
        if (matchesPattern(child->name, pattern)) {
            if ((child->isDirectory && searchDirs) || 
                (!child->isDirectory && searchFiles)) {
                results.push_back(currentPath + "/" + child->name + 
                                 (child->isDirectory ? "/" : ""));
            }
        }
        
        // Recursively search subdirectories
        if (child->isDirectory) {
            std::vector<std::string> subResults = findInDirectory(
                child, pattern, 
                currentPath + "/" + child->name,
                searchFiles, searchDirs
            );
            results.insert(results.end(), subResults.begin(), subResults.end());
        }
    }
    
    return results;
}

// Main find function
std::vector<std::string> FileSystem::find(const std::string& pattern, 
                                         bool searchFiles, 
                                         bool searchDirs) {
    std::vector<std::string> results;
    
    // Start search from current directory
    std::string startPath = (currentDir == root) ? "" : getFullPath(currentDir);
    if (startPath.empty()) startPath = "/";
    
    results = findInDirectory(currentDir, pattern, startPath, searchFiles, searchDirs);
    
    // Remove leading "./" if present and sort
    for (auto& path : results) {
        if (path.substr(0, 2) == "./") {
            path = path.substr(2);
        }
    }
    
    std::sort(results.begin(), results.end());
    return results;
}

