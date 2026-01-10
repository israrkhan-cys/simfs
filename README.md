# simfs (v1.0)

<p align="center">
  <strong>Linux File System Simulator in C++</strong><br>
  <em>Tree data structures made tangible</em>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C++-17-blue">
  <img src="https://img.shields.io/badge/Data%20Structures-Tree%2C%20Vector%2C%20Stack-yellow">
  <img src="https://img.shields.io/badge/Platform-Linux%2FUnix-green">
  <img src="https://img.shields.io/badge/License-MIT-red">
</p>

---

## 🎯 What is simfs?

**simfs** is a Linux file system simulator built from scratch in C++. It started from a simple observation: Linux's hierarchical file system is essentially a tree data structure.

When you navigate `/home/user/docs/file.txt`, you're traversing a tree:

This project implements that tree structure practically, with real file operations and some API integrations as bonus features.

## ✨ Features

### 🗂️ Core File System
- Hierarchical tree-based file storage
- Complete file operations (CRUD, move, copy)
- Path resolution with absolute/relative paths
- Memory management with RAII principles


### 🛠️ Technical Highlights
- Modular C++ architecture
- libcurl for HTTP requests
- Custom JSON parsing
- Build automation
- Terminal UI with ANSI colors


```bash
# Clone and build
git clone https://github.com/israrkhan-cys/simfs.git
cd simfs
./build.sh

# Run
./linux_sim
  📝 License

This project is licensed under the MIT License - see the LICENSE file for details.
