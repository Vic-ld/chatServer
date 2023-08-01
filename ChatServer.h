#pragma once

#include <iostream>
#include <sys/epoll.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>
#include <sys/file.h>
#include <errno.h>
#include <string.h>
#include <cstring>
#include <vector>
#include <map>
#include <algorithm>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "Message.h"

class ChatServer
{
public:
	ChatServer(int port) :m_port(port), m_msg(Message()){};
	~ChatServer() { close(m_serverSocket); };

	void start();
	void init();
	void send(const char* msg, int len );
	void handleMsg(int fd,const char* msg, int len);
	void updateInfo();
	void delFd(int fd);
	int recvMsg(int fd, char** buf);

private:
	int m_serverSocket;
	int m_port;
	std::vector<int> m_clientSockets;
	std::map<std::string, int> m_name2ID;
	Message m_msg;
};

