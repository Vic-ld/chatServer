#include "ChatServer.h"
#include "common.h"
//std::string jsondata = R"({"flag":0,"name":"xxx","msg":"test","target":"xx","ChatInfo":{"total":1,"names":["ld"]}})";

void ChatServer::start()
{
    init();
    epoll_event event;
    int epfd, event_cnt;
    epfd = epoll_create(1);
    if (epfd == -1)
    {
        printf("epoll_create error %d %s", errno, strerror(errno));
    }
    epoll_event* all_events = new epoll_event[100];
    event.events = EPOLLIN;
    event.data.fd = m_serverSocket;
    epoll_ctl(epfd, EPOLL_CTL_ADD, m_serverSocket, &event);

    char *buf;

    int clnt_sock;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_sz = sizeof(clnt_addr);
    memset(&clnt_addr, 0, sizeof(clnt_addr));

    while (true)
    {
        event_cnt = epoll_wait(epfd, all_events, 100, 1000);

        if (event_cnt == -1)
        {
            printf("epoll_wait error %d %s", errno, strerror(errno));
            close(epfd);
        }
        if (event_cnt == 0)continue;

        for (int i = 0; i < event_cnt; i++)
        {
            memset(buf, 0, sizeof(buf));
            if (all_events[i].data.fd == m_serverSocket) //新接
            {
                clnt_addr_sz = sizeof(clnt_addr);
                clnt_sock = accept(m_serverSocket, (sockaddr*)&clnt_addr, &clnt_addr_sz);
                event.events = EPOLLIN;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                m_clientSockets.push_back(clnt_sock);
                printf("client is connected:%d\n", clnt_sock);
                //write(clnt_sock, jsondata.c_str(), jsondata.size());
            }
            else //客服端套接字
            {
                int len = recvMsg(all_events[i].data.fd,&buf);
                if (len <= 0)
                {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, all_events[i].data.fd, NULL);
                    close(all_events[i].data.fd);
                    delFd(all_events[i].data.fd);        
                }
                else
                {

                    handleMsg(all_events[i].data.fd,buf, len);
                   //printf("receive from client:%s\n", buf);
                   // send(buf, len);
                }
            }
        }

    }
}


void ChatServer::handleMsg(int fd, const char* msg, int len)
{
    m_msg.fromStr(msg);
   /* if (m_msg.name == "ld")
    {
        std::cout << "join sleep" << std::endl;
        sleep(5);
        std::cout << "wake up" << std::endl;
    }*/
    switch (m_msg.flag)
    {
    case 0:
        m_name2ID.insert(std::make_pair(m_msg.name, fd)); //绑定 id2socket
        updateInfo();//发送聊天室信息
        break;

    case 1://群发
    {
        send(msg, len);
        break;
    }

    case 2://私聊
    {
        int target = m_name2ID.find(m_msg.target)->second;
        writen(target, msg, len);
        break;
    }

    default:
        break;
    }

}

void ChatServer::updateInfo()
{
    m_msg << m_name2ID;
    m_msg.flag = 4;
    m_msg.name = "server";
    const char* buffer = m_msg.toJsonStr().c_str();
    //std::cout << "updateInfo====" << m_msg.toJsonStr().c_str();
    send(m_msg.toJsonStr().c_str(), strlen(m_msg.toJsonStr().c_str()));
}

void ChatServer::delFd(int fd)
{
    //从sockets列表删除
    auto it = std::find(m_clientSockets.begin(), m_clientSockets.end(), fd);
    m_clientSockets.erase(it);

    //从name到clientid的map中删除
    /*m_name2ID.erase(std::remove_if(m_name2ID.begin(), m_name2ID.end(),
        [&](const auto& pair) { return pair.second == fd; }), m_name2ID.end());*/
    for (auto it = m_name2ID.begin(); it != m_name2ID.end(); )
    {
        if (it->second == fd) {
            it = m_name2ID.erase(it);
            break;
        }
        else {
            ++it;
        }
    }
    printf("client is closed:%d\n", fd);
    updateInfo();
}

int ChatServer::recvMsg(int fd, char**buf)
{
    int netlen = 0;
    int ret = readn(fd, (char*)&netlen, sizeof(int));
    int len = ntohl(netlen);
    std::cout << "len ==" << len << std::endl;
    char* tmp = new char[len + 1];
    ret = readn(fd, tmp, len);
    tmp[len] = '\0';
    *buf = tmp;
    std::cout << "buf ==" << *buf << std::endl;
    return ret;
}


void ChatServer::init()
{
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return ;
    }
    socklen_t optlen;
    int option;
    optlen = sizeof(option);
    option = true;
    setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, optlen);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(m_port);

    if (bind(m_serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Error binding socket to port" << std::endl;
        close(m_serverSocket);
        return ;
    }

    if (listen(m_serverSocket, 5) < 0) {
        std::cerr << "Error listening on socket" << std::endl;
        close(m_serverSocket);
        return ;
    }
}

void ChatServer::send(const char *msg, int len )
{
    for (auto& clnt : m_clientSockets)
    {
       // std::cout << "clnt==" << clnt << std::endl;
        writen(clnt, msg, len);
    }
}

