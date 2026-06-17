#include <iostream>
#include <cstring>
#include <vector>

#include "../common/socket_utils.h"
#include "../common/protocol.h"
#include "chunk_store.h"

int main()
{
    // Register with master
    TcpSocket socket;

    if (!socket.create())
    {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    if (!socket.connectTo("127.0.0.1", 9000))
    {
        std::cerr << "Cannot connect to master\n";
        return 1;
    }

    MessageHeader header;
    header.type =
        static_cast<uint32_t>(
            MessageType::REGISTER_NODE);

    header.payloadSize =
        sizeof(RegisterNodePayload);

    RegisterNodePayload payload{};

    strcpy(payload.ip, "127.0.0.1");
    payload.port = 5001;

    socket.sendAll(&header, sizeof(header));
    socket.sendAll(&payload, sizeof(payload));

    std::cout << "Node registered\n";

    // Create chunk store
    ChunkStore store("data/node1");

    // Start storage server
    TcpSocket storageServer;

    storageServer.create();

    if(!storageServer.create())
{
    std::cerr << "Storage socket creation failed\n";
    return 1;
}


    if (!storageServer.bindAndListen(5001))
    {
        std::cerr << "Cannot listen on 5001\n";
        return 1;
    }

    std::cout << "Storage node listening on 5001\n";

    while (true)
    {
        int clientFd =
            storageServer.acceptConnection();

        if (clientFd < 0)
            continue;

        TcpSocket client(clientFd);

        MessageHeader msgHeader;

        if (!client.recvAll(
                &msgHeader,
                sizeof(msgHeader)))
        {
            continue;
        }

        if (msgHeader.type ==
            static_cast<uint32_t>(
                MessageType::STORE_CHUNK))
        {
            StoreChunkHeader chunkHeader;

            if (!client.recvAll(
                    &chunkHeader,
                    sizeof(chunkHeader)))
            {
                continue;
            }

            std::vector<char> data(
                chunkHeader.dataSize);

            if (!client.recvAll(
                    data.data(),
                    chunkHeader.dataSize))
            {
                continue;
            }

            bool success =
                store.saveChunk(
                    chunkHeader.chunkId,
                    data);

            std::cout
                << "Stored chunk: "
                << chunkHeader.chunkId
                << '\n';

            MessageHeader response;

            response.type =
                static_cast<uint32_t>(
                    success
                    ? MessageType::SUCCESS
                    : MessageType::ERROR_MSG);

            response.payloadSize = 0;

            client.sendAll(
                &response,
                sizeof(response));
        }
        else if(
            msgHeader.type ==
            static_cast<uint32_t>(
                MessageType::GET_CHUNK))
    {
        char chunkId[128];

if(!client.recvAll(
    chunkId,
    sizeof(chunkId)))
{
    continue;
}
auto data =
    store.loadChunk(
        chunkId);
        StoreChunkHeader header{};

std::strncpy(
    header.chunkId,
    chunkId,
    sizeof(header.chunkId)-1);

header.dataSize =
    static_cast<uint32_t>(
        data.size());
        client.sendAll(
    &header,
    sizeof(header));

client.sendAll(
    data.data(),
    data.size());
}
    }
    }
