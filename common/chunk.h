#pragma once

#include <string>
#include <vector>
using namespace std;

struct ChunkLocation{
    string chunkId;
    string nodeIp;
    int nodePort;
};

struct FileMetadata{
    string filename;
    uint64_t fileSize;
    vector<ChunkLocation> chunks;
};