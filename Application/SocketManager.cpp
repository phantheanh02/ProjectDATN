#include "stdafx.h"
#include <iostream>
#include "SocketManager.h"

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
    CloseSocket();
    if (m_socket)
    {
        closesocket(m_socket);
        WSACleanup();
    }
}

bool SocketManager::CreateSocket(bool host, unsigned int port)
{
    Init();
    m_host = host;
    m_port = port;
    m_hasNewMsg = false;
    memset(m_buf, 0, BUFLEN);

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_socket == INVALID_SOCKET) 
    {
        std::cerr << "socket failed\n";
        WSACleanup();
        return 0;
    }
    
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, IP_ADDRESS, &m_serverAddr.sin_addr);

    if (m_host)
    {
        // Bind address to socket
        if (bind(m_socket, (sockaddr*)&m_serverAddr, sizeof(m_serverAddr)))
        {
            printf("Error! Cannot bind this address.\n");
            CloseSocket();
            return false;
        }

        // listen reqest from client
        if (listen(m_socket, 1))
        {
            printf("Error listening\n");
            return 0;
        }
        printf("Server started!\n");
    }
    else
    {
        if (connect(m_socket, (sockaddr*)&m_serverAddr, sizeof(m_serverAddr)))
        {
            printf("Error! Cannot connect server.");
            return false;
        }
        // Ping to server
        SendNewMessage("action00"); // CONNECT
        int ret = recv(m_socket, m_buf, BUFLEN, 0);
        if (ret == SOCKET_ERROR) 
        {
            if (WSAGetLastError() == WSAETIMEDOUT)
            {
                printf("Time-out!\n");
            }
            else
            {
                printf("Error! Cannot receive message from server\n");
            }
            CloseSocket();
            return false;
        }
        else 
        {
            m_buf[ret] = 0;
        }
        printf("Client started!\n");
    }
    CreateReceiveThread();
}

void SocketManager::CreateReceiveThread()
{
    m_threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, &m_socket, 0, NULL);
    if (m_threadHandle == NULL)
    {
        perror("Error creating thread\n");
        return;
    }
}

void SocketManager::CreateNewThread(LPTHREAD_START_ROUTINE func)
{
    if (m_threadHandle)
    {
        CloseHandle(m_threadHandle);
        m_threadHandle = nullptr;
    }
     
    m_threadHandle = CreateThread(NULL, 0, func, &m_socket, 0, NULL);
    if (m_threadHandle == NULL)
    {
        perror("Error creating thread\n");
        return;
    }
}

void SocketManager::CloseSocket()
{
    // Shutdown the socket
    int result = shutdown(m_socket, SD_BOTH);
    if (result == SOCKET_ERROR) 
    {
        std::cerr << "shutdown failed\n";
    }

    // Close the thread
    if (m_threadHandle)
    {
        CloseHandle(m_threadHandle);
        m_threadHandle = nullptr;
    }
    WSACleanup();

    std::cout << "Closed socket\n";
}

void SocketManager::CloseClientSocket()
{
    if (m_threadHandle)
    {
        CloseHandle(m_threadHandle);
        m_threadHandle = nullptr;
    }
    m_threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, &m_socket, 0, NULL);
    if (m_threadHandle == NULL)
    {
        perror("Error creating thread\n");
        return;
    }
}

void SocketManager::CloseReceiveThread()
{
    if (m_threadHandle)
    {
        CloseHandle(m_threadHandle);
        m_threadHandle = nullptr;
    }
}


bool SocketManager::SendNewMessage(const char* message)
{
    //if (!message)
    //{
    //    return;
    //}

    int ret = SOCKET_ERROR;
    if (m_host)
    {
        //ret = sendto(m_socket, message, (int)strlen(message), 0, (SOCKADDR*)&m_clientAddr, sizeof(m_clientAddr));
        ret = send(m_clientSock, message, (int)strlen(message), 0);
    }
    else
    {
        //ret = sendto(m_socket, message, (int)strlen(message), 0, (SOCKADDR*)&m_serverAddr, sizeof(m_serverAddr));
        ret = send(m_socket, message, (int)strlen(message), 0);
    }

    if (ret == SOCKET_ERROR)
    {
        printf("Error when send\n");
        return false;
    }
    else
    {
        //printf("Send a message: %s\n", message);
    }
    return true;
}

bool SocketManager::ReceiveMessage()
{
    if (m_host)
    {
        int clientAddrSize = sizeof(m_clientAddr);
        //int ret = recvfrom(m_socket, m_buf, BUFLEN, 0, (sockaddr*)&m_clientAddr, &clientAddrSize);
        int ret = recv(m_clientSock, m_buf, 8, MSG_WAITALL);
        if (ret == SOCKET_ERROR)
        {
            memset(m_buf, 0, BUFLEN);
            strcpy(m_buf, "action99");
            printf("Error receive at server\n");
            return false;
        }
        
        if (ret == 0)
        {
            CloseClientSocket();
            return 0;
        }
        m_buf[ret] = 0;
        //printf("Receive from client: %s\n", m_buf);
    }
    else
    {
        int serverAddrSize = sizeof(m_serverAddr);
        //int ret = recvfrom(m_socket, m_buf, BUFLEN, 0, (sockaddr*)&m_serverAddr, &serverAddrSize);
        int ret = recv(m_socket, m_buf, 8, MSG_WAITALL);
        if (ret == SOCKET_ERROR)
        {
            memset(m_buf, 0, BUFLEN);
            strcpy(m_buf, "action99");
            printf("Error receive at client\n");
            return false;
        }
        m_buf[ret] = 0;
        //printf("Receive from server: %s\n", m_buf);
    }
    m_hasNewMsg = true;
    return true;
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

bool SocketManager::IsHost()
{
    return m_host;
}

void SocketManager::SetHost(bool host)
{
    m_host = host;
}

void SocketManager::ThreadProc(void* lpParameter)
{
    LPVOID* param = reinterpret_cast<LPVOID*>(lpParameter);

    if (SocketManager::GetInstance()->IsHost())
    {
        SocketManager::GetInstance()->AcceptClientConnect();
    }

    while (SocketManager::GetInstance()->ReceiveMessage()){}
}

void SocketManager::AcceptClientConnect()
{
    int clientAddrLen = sizeof(m_clientAddr);
    //accept connect
    m_clientSock = accept(m_socket, (sockaddr*)&m_clientAddr, &clientAddrLen);
    //inet_ntop(AF_INET, &m_clientAddr.sin_addr, IP_ADDRESS, sizeof(IP_ADDRESS));
    //ntohs(m_clientAddr.sin_port);
    printf("Connectd with client\n");
}



