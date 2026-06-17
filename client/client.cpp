#include <iostream>
#include "../common/socket_utils.h"
using namespace std;

int main(){
    TcpSocket client;
    if(!client.create()){
        cerr << "Socket creation failed\n";
        return 1;
    }
    if(!client.connectTo(
        "127.0.0.1",
        9000
    )){
        cerr << "Connection failed\n";
        return 1;
    }
    cout << "Connected To master\n";
    return 0;
}