#include "file_chunker.h"

#include <fstream>

std::vector<Chunk>
FileChunker::splitFile(
    const std::string& filename,
    size_t chunkSize)
{
    std::ifstream file(
        filename,
        std::ios::binary);

    if(!file)
        return {};

    std::vector<Chunk> chunks;

    int index = 0;

    while(true)
    {
        std::vector<char> buffer(
            chunkSize);

        file.read(
            buffer.data(),
            chunkSize);

        std::streamsize bytesRead =
            file.gcount();

        if(bytesRead <= 0)
            break;

        buffer.resize(bytesRead);

        Chunk chunk;

        chunk.id =
            "chunk_" +
            std::to_string(index++);

        chunk.data =
            std::move(buffer);

        chunks.push_back(
            std::move(chunk));
    }

    return chunks;
}