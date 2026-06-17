#include <iostream>
#include <vector>
#include <cstring>

#include "../common/socket_utils.h"
#include "../common/protocol.h"
#include "../common/file_chunker.h"

bool sendChunk(
    const Chunk& chunk,
    const char* ip,
    int port)
{
    TcpSocket socket;

    if (!socket.create())
        return false;

    if (!socket.connectTo(ip, port))
        return false;

    MessageHeader header;

    header.type =
        static_cast<uint32_t>(
            MessageType::STORE_CHUNK);

    header.payloadSize =
        sizeof(StoreChunkHeader)
        + static_cast<uint32_t>(
            chunk.data.size());

    StoreChunkHeader chunkHeader{};

    std::strncpy(
        chunkHeader.chunkId,
        chunk.id.c_str(),
        sizeof(chunkHeader.chunkId) - 1);

    chunkHeader.dataSize =
        static_cast<uint32_t>(
            chunk.data.size());

    if (!socket.sendAll(
            &header,
            sizeof(header)))
        return false;

    if (!socket.sendAll(
            &chunkHeader,
            sizeof(chunkHeader)))
        return false;

    if (!socket.sendAll(
            chunk.data.data(),
            chunk.data.size()))
        return false;

    MessageHeader response;

    if (!socket.recvAll(
            &response,
            sizeof(response)))
        return false;

    return response.type ==
           static_cast<uint32_t>(
               MessageType::SUCCESS);
}

int main(
    int argc,
    char* argv[])
{
    if (argc != 2)
    {
        std::cout
            << "Usage: ./upload_client <file>\n";

        return 1;
    }

    auto chunks =
        FileChunker::splitFile(
            argv[1]);

    if (chunks.empty())
    {
        std::cerr
            << "Failed to read file\n";

        return 1;
    }

    TcpSocket master;

    if (!master.create())
    {
        std::cerr
            << "Master socket creation failed\n";

        return 1;
    }

    if (!master.connectTo(
            "127.0.0.1",
            9000))
    {
        std::cerr
            << "Cannot connect to master\n";

        return 1;
    }

    UploadRequest request{};

    std::strncpy(
        request.filename,
        argv[1],
        sizeof(request.filename) - 1);

    request.totalChunks =
        static_cast<uint32_t>(
            chunks.size());

    MessageHeader header;

    header.type =
        static_cast<uint32_t>(
            MessageType::UPLOAD_REQUEST);

    header.payloadSize =
        sizeof(UploadRequest);

    if (!master.sendAll(
            &header,
            sizeof(header)))
    {
        return 1;
    }

    if (!master.sendAll(
            &request,
            sizeof(request)))
    {
        return 1;
    }

    std::vector<UploadResponse>
        assignments;

    for (size_t i = 0;
         i < chunks.size();
         i++)
    {
        UploadResponse response;

        if (!master.recvAll(
                &response,
                sizeof(response)))
        {
            std::cerr
                << "Failed to receive assignment\n";

            return 1;
        }

        assignments.push_back(
            response);
    }

    for (size_t i = 0;
         i < chunks.size();
         i++)
    {
        bool success =
            sendChunk(
                chunks[i],
                assignments[i].nodeIp,
                assignments[i].nodePort);

        if (success)
        {
            std::cout
                << "Uploaded "
                << chunks[i].id
                << " -> "
                << assignments[i].nodeIp
                << ":"
                << assignments[i].nodePort
                << '\n';
        }
        else
        {
            std::cout
                << "Failed "
                << chunks[i].id
                << '\n';
        }
    }

    return 0;
}