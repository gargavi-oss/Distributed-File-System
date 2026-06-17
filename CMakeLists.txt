cmake_minimum_required(VERSION 3.16)

project(DFS)

set(CMAKE_CXX_STANDARD 20)

add_executable(
    master
    master/master.cpp
    master/metadata_manager.cpp
)

add_executable(
    client
    client/client.cpp
)

add_executable(
    chunk_sender
    client/chunk_sender.cpp
)

add_executable(
    storage
    storage/storage.cpp
    storage/chunk_store.cpp
)

add_executable(
    test_chunker
    client/test_chunker.cpp
    common/file_chunker.cpp
)

add_executable(
    upload_client
    client/upload_client.cpp
    common/file_chunker.cpp
)

add_executable(
    download_client
    client/download_client.cpp
)