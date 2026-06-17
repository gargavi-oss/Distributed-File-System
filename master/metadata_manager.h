#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include "../common/chunk.h"
using namespace std;


struct StorageNode{
    string ip;
    int port;
    uint64_t usedSpace;
};
class MetadataManager{
    private:
        unordered_map<string,FileMetadata>files;
        vector<StorageNode> nodes;
        mutable mutex mutex_;
    public:
        void registerNode(
            const StorageNode& node
        );
        vector <StorageNode> 
        getNodes() const;
        void addFile(
            const FileMetadata& file
        );
        bool fileExists(
            const string& filename
        ) const;
        FileMetadata getFile(
            const string& filename
        ) const;
        void deleteFile(
            const string& filename
        );
        vector<string>
        listFiles() const;
};
