#include <cstdio>
#include "ChatServer.h"

int main()
{
    ChatServer myServer(9527);
    myServer.start();
    return 0;
}