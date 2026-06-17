#pragma once
#include <cstdint>
#include <string>

enum class MessageType : uint32_t
{
    REGISTER_NODE = 1,

    UPLOAD_REQUEST,
    UPLOAD_RESPONSE,

    STORE_CHUNK,

    DOWNLOAD_REQUEST,
    DOWNLOAD_RESPONSE,

    GET_CHUNK,
    CHUNK_DATA,

    LIST_FILES,
    DELETE_FILE,

    SUCCESS,
    ERROR_MSG
};

struct MessageHeader
{
    uint32_t type;
    uint32_t payloadSize;
};

struct StoreChunkHeader
{
    char chunkId[128];
    uint32_t dataSize;
};

struct RegisterNodePayload
{
    char ip[64];

    uint32_t port;
};


struct UploadRequest
{
    char filename[256];

    uint32_t totalChunks;
};

struct UploadResponse
{
    char chunkId[128];

    char nodeIp[64];

    uint32_t nodePort;
};

struct DownloadRequest{
    char filename[256];
};

struct DownloadResponse
{
    char chunkId[128];

    char nodeIp[64];

    uint32_t nodePort;
};