#pragma once
#include "Singleton.h"
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define BUFLEN 512
#define SERVER_PORT 9000
#define CLIENT_PORT 9001
#define IP_ADDRESS "127.0.0.1"

class SocketManager : public SingletonDclp<SocketManager>
{
public:
	bool Init();
	void CleanUp();

	bool CreateSocket(bool host, unsigned int port);
	void BindSocket();
	void SendMessage(const char* message);
	void* ReceiveMessage();

	bool HasNewMsg();
	void SetStatusMsg(bool isReceives);
	std::string  GetDataMsg();
private:
	bool	m_host;
	int		m_port;

	WSADATA m_wsaData;
	SOCKET	m_socket;
	char	m_buf[BUFLEN];
	//struct addrinfo m_hints, * m_addrInfo;
	sockaddr_in m_serverAddr, m_clientAddr;

	bool		m_hasNewMsg;

	static void ThreadProc(void* lpParameter);
};