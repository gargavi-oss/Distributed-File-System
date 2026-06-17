#include "chunk_store.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

ChunkStore::ChunkStore(const std::string& path)
    : storagePath(path)
{
    try
    {
        fs::create_directories(storagePath);
    }
    catch(const fs::filesystem_error& e)
    {
        std::cerr
            << "Filesystem error: "
            << e.what()
            << '\n';
    }
}
bool ChunkStore::saveChunk(
    const std::string& chunkId,
    const std::vector<char>& data)
{
    std::ofstream out(
        storagePath + "/" + chunkId,
        std::ios::binary);
    if(!out)
        return false;
    out.write(
        data.data(),
        data.size());
    return true;
}

std::vector<char>
ChunkStore::loadChunk(
    const std::string& chunkId)
{
    std::ifstream in(
        storagePath + "/" + chunkId,
        std::ios::binary);
    if(!in)
        return {};
    return {
        std::istreambuf_iterator<char>(in),
        std::istreambuf_iterator<char>()
    };
}

bool ChunkStore::deleteChunk(
    const std::string& chunkId)
{
    return std::filesystem::remove(
        storagePath + "/" + chunkId);
}