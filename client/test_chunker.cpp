#include <iostream>

#include "../common/file_chunker.h"

int main()
{
    auto chunks =
        FileChunker::splitFile(
            "test.txt",
            8);

    std::cout
        << "Total Chunks: "
        << chunks.size()
        << "\n\n";

    for (const auto& chunk : chunks)
    {
        std::cout
            << chunk.id
            << " -> ";

        for (char c : chunk.data)
        {
            std::cout << c;
        }

        std::cout << '\n';
    }

    return 0;
}