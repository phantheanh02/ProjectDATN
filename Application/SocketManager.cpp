#include "stdafx.h"
#include <iostream>
#include "SocketManager.h"
#include <thread>

bool SocketManager::Init()
{
    // Initialize Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
    if (result != 0)
    {
        printf("WSAStartup failed: %d\n", result);
        return 0;
    }
    return 1;
}

void SocketManager::CleanUp()
{
    //freeaddrinfo(m_addrInfo);
    closesocket(m_socket);
    WSACleanup();
}

bool SocketManager::CreateSocket(bool host, unsigned int port)
{
    m_host = host;
    m_port = port;
    m_hasNewMsg = false;

    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCKET) 
    {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    }
    
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, IP_ADDRESS, &m_serverAddr.sin_addr);

    if (m_host)
    {
        if (bind(m_socket, (sockaddr*)&m_serverAddr, sizeof(m_serverAddr)))
        {
            printf("Error! Cannot bind this address.\n");
            return 0;
        }
        printf("Server started!\n");
    }
    else
    {
        SendMessage("Client has connected\n");
        printf("Client started!\n");
    }
    HANDLE  threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, &m_socket, 0, NULL);
    //if (threadHandle == NULL) 
    //{
    //    perror("Error creating thread");
    //    return 0;
    //}
}

void SocketManager::BindSocket()
{
   /*ZeroMemory(&m_hints, sizeof(m_hints));
    m_hints.ai_family = AF_INET;
    m_hints.ai_socktype = SOCK_DGRAM;
    m_hints.ai_protocol = IPPROTO_UDP;
    m_hints.ai_flags = AI_PASSIVE;

    char port[16];
    memset(port, 0, 16);
    sprintf(port, "%d", m_port);

    int result = getaddrinfo(IP_ADDRESS, port, &m_hints, &m_addrInfo);
    if (result != 0) 
    {
        std::cerr << "getaddrinfo failed: " << result << std::endl;
        closesocket(m_socket);
        WSACleanup();
        return;
    }

    result = bind(m_socket, m_addrInfo->ai_addr, (int)m_addrInfo->ai_addrlen);
    if (result == SOCKET_ERROR) 
    {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(m_addrInfo);
        closesocket(m_socket);
        WSACleanup();
        return;
    }*/
}

void SocketManager::SendMessage(const char* message)
{
    if (!message)
    {
        return;
    }
    //int result = send(m_socket, message, (int)strlen(message), 0);
    //if (result == SOCKET_ERROR) 
    //{
    //    std::cerr << "sendto failed: " << WSAGetLastError() << std::endl;
    //    freeaddrinfo(m_addrInfo);
    //    closesocket(m_socket);
    //    WSACleanup();
    //    return;
    //}
    int ret = SOCKET_ERROR;
    if (m_host)
    {
        ret = sendto(m_socket, message, (int)strlen(message), 0, (SOCKADDR*)&m_clientAddr, sizeof(m_clientAddr));
    }
    else
    {
        ret = sendto(m_socket, message, (int)strlen(message), 0, (SOCKADDR*)&m_serverAddr, sizeof(m_serverAddr));
    }

    if (ret == SOCKET_ERROR)
    {
        printf("Error when send: %\n", WSAGetLastError());
    }
}

void* SocketManager::ReceiveMessage()
{
    while (true)
    {
        memset(m_buf, 0, BUFLEN);

        if (m_host)
        {
            int clientAddrSize = sizeof(m_clientAddr);
            int ret = recvfrom(m_socket, m_buf, BUFLEN, 0, (sockaddr*)&m_clientAddr, &clientAddrSize);
            if (ret == SOCKET_ERROR)
            {
                printf("Error receive from server : %\n", WSAGetLastError());
                return 0;
            }

            m_buf[ret] = 0;
            printf("Receive from client : % s\n", m_buf);
        }
        else
        {
            int serverAddrSize = sizeof(m_serverAddr);
            int ret = recvfrom(m_socket, m_buf, BUFLEN, 0, (sockaddr*)&m_serverAddr, &serverAddrSize);
            if (ret == SOCKET_ERROR)
            {
                printf("Error receive from client : %\n", WSAGetLastError());
                continue;
            }
            m_buf[ret] = 0;
            printf("Receive from server : % s\n", m_buf);
        }
        m_hasNewMsg = true;
    }
}

bool SocketManager::HasNewMsg()
{
    return m_hasNewMsg;
}

void SocketManager::SetStatusMsg(bool isReceives)
{
    m_hasNewMsg = isReceives;
}

std::string SocketManager::GetDataMsg()
{
    return std::string(m_buf);
}

void SocketManager::ThreadProc(void* lpParameter)
{
    LPVOID* param = reinterpret_cast<LPVOID*>(lpParameter);
    SocketManager::GetInstance()->ReceiveMessage();
}



