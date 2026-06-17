#include <iostream>
#include <cstring>
#include "metadata_manager.h"

#include "../common/socket_utils.h"
#include "../common/protocol.h"

int main()
{
    MetadataManager metadata;

    TcpSocket server;

    server.create();

    if(!server.bindAndListen(9000))
    {
        std::cerr
            << "Cannot bind\n";

        return 1;
    }

    std::cout
        << "Master running\n";

    while(true)
    {
        int clientFd =
            server.acceptConnection();

        if(clientFd < 0)
            continue;

        TcpSocket client(clientFd);

        MessageHeader header;

        if(!client.recvAll(
            &header,
            sizeof(header)))
        {
            continue;
        }

        if(
            header.type ==
            static_cast<uint32_t>(
                MessageType::REGISTER_NODE))
        {
            RegisterNodePayload payload;

            if(!client.recvAll(
                &payload,
                sizeof(payload)))
            {
                continue;
            }

            StorageNode node;

            node.ip = payload.ip;

            node.port = payload.port;

            node.usedSpace = 0;

            metadata.registerNode(
                node);

            std::cout
                << "Registered Node: "
                << node.ip
                << ":"
                << node.port
                << '\n';

            std::cout
                << "Total Nodes: "
                << metadata
                    .getNodes()
                    .size()
                << '\n';
        }
       else if(
    header.type ==
    static_cast<uint32_t>(
        MessageType::UPLOAD_REQUEST))
{
    UploadRequest request;

    if(!client.recvAll(
        &request,
        sizeof(request)))
    {
        continue;
    }

    std::cout
        << "Upload Request: "
        << request.filename
        << '\n';

    auto nodes =
        metadata.getNodes();

    if(nodes.empty())
    {
        std::cerr
            << "No storage nodes available\n";

        continue;
    }

    StorageNode node =
        nodes.front();

    FileMetadata file;

    file.filename =
        request.filename;

    file.fileSize = 0;

    for(uint32_t i = 0;
        i < request.totalChunks;
        i++)
    {
        UploadResponse response{};

        std::string chunkId =
            "chunk_" +
            std::to_string(i);

        std::strncpy(
            response.chunkId,
            chunkId.c_str(),
            sizeof(response.chunkId) - 1);

        std::strncpy(
            response.nodeIp,
            node.ip.c_str(),
            sizeof(response.nodeIp) - 1);

        response.nodePort =
            node.port;

        ChunkLocation location;

        location.chunkId =
            chunkId;

        location.nodeIp =
            node.ip;

        location.nodePort =
            node.port;

        file.chunks.push_back(
            location);

        client.sendAll(
            &response,
            sizeof(response));
    }

    metadata.addFile(file);

    std::cout
        << "Assigned "
        << request.totalChunks
        << " chunks\n";

    std::cout
        << "Stored metadata for "
        << file.filename
        << '\n';

    auto files =
        metadata.listFiles();

    std::cout
        << "\nFiles:\n";

    for(const auto& f : files)
    {
        std::cout
            << " - "
            << f
            << '\n';
    }
}
else if(
    header.type ==
    static_cast<uint32_t>(
        MessageType::DOWNLOAD_REQUEST))
{
    DownloadRequest request;

    if(!client.recvAll(
        &request,
        sizeof(request)))
    {
        continue;
    }

    FileMetadata file =
        metadata.getFile(
            request.filename);

    for(const auto& chunk :
        file.chunks)
    {
        DownloadResponse response{};

        std::strncpy(
            response.chunkId,
            chunk.chunkId.c_str(),
            sizeof(response.chunkId)-1);

        std::strncpy(
            response.nodeIp,
            chunk.nodeIp.c_str(),
            sizeof(response.nodeIp)-1);

        response.nodePort =
            chunk.nodePort;

        client.sendAll(
            &response,
            sizeof(response));
    }

    std::cout
        << "Download request: "
        << request.filename
        << '\n';
}
    }
}