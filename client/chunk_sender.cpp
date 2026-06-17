#include <iostream>
#include <cstring>

#include "../common/socket_utils.h"
#include "../common/protocol.h"

int main()
{
    TcpSocket socket;

    socket.create();

    if(!socket.connectTo(
        "127.0.0.1",
        5001))
    {
        std::cout << "Cannot connect\n";
        return 1;
    }

    std::string text =
        "Hello DFS";

    MessageHeader header;

    header.type =
        static_cast<uint32_t>(
            MessageType::STORE_CHUNK);

    header.payloadSize =
        sizeof(StoreChunkHeader)
        + text.size();

    StoreChunkHeader chunk{};

    strcpy(
        chunk.chunkId,
        "chunk_001");

    chunk.dataSize =
        static_cast<uint32_t>(
            text.size());

    socket.sendAll(
        &header,
        sizeof(header));

    socket.sendAll(
        &chunk,
        sizeof(chunk));

    socket.sendAll(
        text.data(),
        text.size());

    std::cout
        << "Chunk sent\n";
}