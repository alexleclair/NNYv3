#ifndef H_ZSOCKET
#define H_ZSOCKET

#include "Packet.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/Log_Msg.h"

#define SOCKET_READ_BUFFER_SIZE 1024
#define SOCKET_TIMEOUT 10

class Socket {
public:
	Socket();
	~Socket();
	bool		connect(const char *host, unsigned short port);
	void		close();
	bool		write(Packet &data);
	Packet      read();
private:
	ACE_SOCK_Connector  m_connector;
	ACE_SOCK_Stream		m_socket;
};

#endif
