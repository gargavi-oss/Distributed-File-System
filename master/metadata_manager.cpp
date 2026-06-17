#include "metadata_manager.h"
using namespace std;

void MetadataManager::registerNode(
    const StorageNode& node)
{
    std::lock_guard<std::mutex>
        lock(mutex_);

    for(const auto& existing : nodes)
    {
        if(existing.ip == node.ip &&
           existing.port == node.port)
        {
            return;
        }
    }

    nodes.push_back(node);
}
vector<StorageNode>
MetadataManager::getNodes() const{
    lock_guard<mutex>
       lock(mutex_);
    return nodes; 
}

void MetadataManager::addFile(
    const FileMetadata& file)
{
    lock_guard<mutex>
        lock(mutex_);

    files[file.filename] = file;
}

bool MetadataManager::fileExists(
    const string& filename) const
{
    lock_guard<mutex>
        lock(mutex_);
    return files.find(filename)
           != files.end();
}

FileMetadata MetadataManager::getFile(
    const std::string& filename) const
{
    std::lock_guard<std::mutex>
        lock(mutex_);

    auto it =
        files.find(filename);

    if(it != files.end())
        return it->second;

    return {};
}
void MetadataManager::deleteFile(
    const string& filename)
{
    lock_guard<mutex>
        lock(mutex_);
    files.erase(filename);
}

vector<string>
MetadataManager::listFiles() const
{
    lock_guard<mutex>
        lock(mutex_);
    vector<string>
        result;
    for(const auto& [name, _] : files)
    {
        result.push_back(name);
    }
    return result;
}