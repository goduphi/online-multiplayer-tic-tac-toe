#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>

#define BUFFSIZE 4
#define MAX_CLIENTS 2

// This Clients struct will hold all clients and will be passed into the thread function
typedef struct Clients
{
	uint8_t id;
	int descriptors[MAX_CLIENTS];
	char *data;
} Clients;

typedef struct sockaddr_in SocketAddress;

void SetupIpPort(char ip[], in_port_t *port, char *argv[]);
int CreateSyncSocket();
void ConstructServerAddrPort(struct sockaddr_in *servAddr, char ip[], in_port_t port);
void Send(int socketDescriptor, char string[]);
void ReceiveData(int socketDescriptor, char string[]);
void ConstructServerPort(struct sockaddr_in *servAddr, in_port_t port);
ssize_t ServerReceiveData(int socketDescriptor, char string[]);
void ServerSendData(int socketDescriptor, char string[], ssize_t bytesReceived);

#endif