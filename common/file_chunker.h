#pragma once

#include <string>
#include <vector>

struct Chunk
{
    std::string id;

    std::vector<char> data;
};

class FileChunker
{
public:

    static std::vector<Chunk>
    splitFile(
        const std::string& filename,
        size_t chunkSize = 64 * 1024
    );
};