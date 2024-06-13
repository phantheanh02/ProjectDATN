#pragma once
#include "Singleton.h"
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

#define BUFLEN 512
#define SERVER_PORT 9000
#define CLIENT_PORT 9001
#define IP_ADDRESS "127.0.0.1"

enum RequestType
{
	CONNECT			= 00,

	READY_STATE		= 10,
	START_PLAY		= 11,
	READY			= 12,
	NO_READY		= 13,
	JOIN_BATTLE		= 14,
	EXIT_ROOM		= 15,
	EXIT_BATTLE		= 16,

	CHARACTER_BLACK = 20,
	CHARACTER_BLUE	= 21,
	CHARACTER_GREEN	= 22,
	CHARACTER_RED	= 23,
	CHARACTER_YELLOW = 24,

	DISCONNECT		= 99
};
class SocketManager : public SingletonDclp<SocketManager>
{
public:
	bool Init();
	void CleanUp();

	bool CreateSocket(bool host, unsigned int port);
	void CreateReceiveThread();
	void CreateNewThread(LPTHREAD_START_ROUTINE func);

	void CloseSocket();
	void CloseClientSocket();
	void CloseReceiveThread();

	bool SendNewMessage(const char* message);
	bool ReceiveMessage();

	bool HasNewMsg();
	void SetStatusMsg(bool isReceives);
	std::string  GetDataMsg();

	bool IsHost();
	void SetHost(bool host);
private:
	bool	m_host;
	int		m_port;

	WSADATA m_wsaData;
	SOCKET	m_socket;
	//SOCKET	m_listenSock;
	SOCKET	m_clientSock;

	char	m_buf[BUFLEN];
	//struct addrinfo m_hints, * m_addrInfo;
	sockaddr_in m_serverAddr, m_clientAddr;

	bool		m_hasNewMsg;
	HANDLE		m_threadHandle;

private:
	static void ThreadProc(void* lpParameter);
	void AcceptClientConnect();
};