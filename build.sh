#!/bin/bash

sudo rm -rf build
mkdir -p build

echo "🔨 Compiling source files..."

# Compiling each .cpp file separately first 
g++ -std=c++17 -I./include -c src/utils.cpp -o build/utils.o
g++ -std=c++17 -I./include -c src/fs_node.cpp -o build/fs_node.o
g++ -std=c++17 -I./include -c src/file_system.cpp -o build/file_system.o
g++ -std=c++17 -I./include -c src/weather.cpp -o build/weather.o
g++ -std=c++17 -I./include -c src/crypto.cpp -o build/crypto.o  
g++ -std=c++17 -I./include -c main.cpp -o build/main.o

# Link the compiled shit together 
echo "🔗 Linking..."
g++ build/utils.o build/fs_node.o build/file_system.o build/weather.o build/crypto.o build/main.o -lcurl -o simfs

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "==>Run with: ./simfs"

else
    echo "❌ Build failed!"
    exit 1
fi