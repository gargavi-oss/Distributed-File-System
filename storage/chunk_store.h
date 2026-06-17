#pragma once

#include <string>
#include <vector>

class ChunkStore
{
private:
    std::string storagePath;

public:
    explicit ChunkStore(
        const std::string& path
    );
    bool saveChunk(
        const std::string& chunkId,
        const std::vector<char>& data
    );
    std::vector<char> loadChunk(
        const std::string& chunkId
    );

    bool deleteChunk(
        const std::string& chunkId
    );
};