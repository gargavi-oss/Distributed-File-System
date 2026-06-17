#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#include "../common/socket_utils.h"
#include "../common/protocol.h"

std::vector<char>
getChunk(
    const char* chunkId,
    const char* ip,
    int port)
{
    TcpSocket socket;

    if(!socket.create())
    {
        return {};
    }

    if(!socket.connectTo(
        ip,
        port))
    {
        return {};
    }

    MessageHeader header;

    header.type =
        static_cast<uint32_t>(
            MessageType::GET_CHUNK);

    header.payloadSize = 128;

    if(!socket.sendAll(
        &header,
        sizeof(header)))
    {
        return {};
    }

    if(!socket.sendAll(
        chunkId,
        128))
    {
        return {};
    }

    StoreChunkHeader response;

    if(!socket.recvAll(
        &response,
        sizeof(response)))
    {
        return {};
    }

    std::vector<char> data(
        response.dataSize);

    if(!socket.recvAll(
        data.data(),
        response.dataSize))
    {
        return {};
    }

    return data;
}

int main(
    int argc,
    char* argv[])
{
    if(argc != 2)
    {
        std::cout
            << "Usage: ./download_client <filename>\n";

        return 1;
    }

    TcpSocket master;

    if(!master.create())
    {
        std::cerr
            << "Failed to create socket\n";

        return 1;
    }

    if(!master.connectTo(
        "127.0.0.1",
        9000))
    {
        std::cerr
            << "Cannot connect to master\n";

        return 1;
    }

    DownloadRequest request{};

    std::strncpy(
        request.filename,
        argv[1],
        sizeof(request.filename) - 1);

    MessageHeader header;

    header.type =
        static_cast<uint32_t>(
            MessageType::DOWNLOAD_REQUEST);

    header.payloadSize =
        sizeof(request);

    if(!master.sendAll(
        &header,
        sizeof(header)))
    {
        std::cerr
            << "Failed to send request\n";

        return 1;
    }

    if(!master.sendAll(
        &request,
        sizeof(request)))
    {
        std::cerr
            << "Failed to send filename\n";

        return 1;
    }

    DownloadResponse response;

    if(!master.recvAll(
        &response,
        sizeof(response)))
    {
        std::cerr
            << "Failed to receive metadata\n";

        return 1;
    }

    std::cout
        << "Chunk: "
        << response.chunkId
        << " -> "
        << response.nodeIp
        << ":"
        << response.nodePort
        << '\n';

    auto data =
        getChunk(
            response.chunkId,
            response.nodeIp,
            response.nodePort);

    if(data.empty())
    {
        std::cerr
            << "Failed to download chunk\n";

        return 1;
    }

    std::ofstream output(
        "downloaded.txt",
        std::ios::binary);

    if(!output)
    {
        std::cerr
            << "Failed to create output file\n";

        return 1;
    }

    output.write(
        data.data(),
        data.size());

    output.close();

    std::cout
        << "Downloaded successfully\n";

    std::cout
        << "Saved as downloaded.txt\n";

    return 0;
}