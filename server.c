#include <stdio.h>
#include <pthread.h>
#include "socket.h"

static const int MAXPENDING = 2; // Maximum number of outstanding connection requests
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct client_info
{
	int socket_desp;
	SocketAddress socketAddr;
};

void CheckCmdArgs(int argc, char *argv[])
{
	if(argc < 1 || argc > 2)
	{
		printf("Follow the format: %s <Server Address> <Port Number>", argv[0]);
		exit(0);
	}
}

void * process_client_request(void * ci)
{
	struct client_info c_i = *((struct client_info *)ci);
	SocketAddress ClientAddr = c_i.socketAddr;
	int csd = c_i.socket_desp;
	
	pthread_mutex_lock(&lock);
	
	char ClientName[INET_ADDRSTRLEN];
		
	if(inet_ntop(AF_INET, &ClientAddr.sin_addr.s_addr, ClientName, sizeof(ClientName)) != NULL)
		printf("Handling client %s/%d\n", ClientName, ntohs(ClientAddr.sin_port));
	else
		printf("Unable to get client address");
	
	char Msg[BUFFSIZE - 1];
	memset(Msg, 0, sizeof(Msg));
	
	pthread_mutex_unlock(&lock);
	
	// Handle client data
	ssize_t bytesReceived = ServerReceiveData(csd, Msg);
	ServerSendData(csd, Msg, bytesReceived);
	
	close(csd); // Close the client connection
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	// Check for proper number of arguments
	CheckCmdArgs(argc, argv);
	
	in_port_t Port = atoi(argv[1]);	// Local port
	
	// Create a synchronous socket
	int SocketDescriptor = CreateSyncSocket();
	
	SocketAddress ServAddr;
	ConstructServerPort(&ServAddr, Port);
	
	// Bind to the local address
	if(bind(SocketDescriptor, (struct sockaddr *)&ServAddr, sizeof(ServAddr)) < 0)
		perror("bind() failed");
	
	// Mark the socket so it will listen for incoming connections
	if(listen(SocketDescriptor, MAXPENDING) < 0)
		perror("listen() failed");
	
	pthread_t client_request_tid[MAXPENDING];
	int client_request_idx = 0;
	
	// You want to accept all incoming connections
	while(1)
	{
		SocketAddress ClientAddr;	// Client Address
		
		// Set the length of the client address structure
		socklen_t ClientAddrLen = sizeof(ClientAddr);
		
		// Wait for client connect
		int ClientSock = accept(SocketDescriptor, (struct sockaddr *)&ClientAddr, &ClientAddrLen);
		if(ClientSock < 0)
		{
			perror("accept() failed");
			exit(0);
		}
		
		struct client_info ci;
		ci.socket_desp = ClientSock;
		ci.socketAddr = ClientAddr;
		
		// For each client, create a new thread to process that request
		// This allows for main to accept more clients
		if(pthread_create(&client_request_tid[client_request_idx], NULL, process_client_request, (struct client_info *)&ci))
		{
			printf("Thread creation failed.\n");
		}
		
		// Join all of spawned the threads
		// https://dzone.com/articles/parallel-tcpip-socket-server-with-multi-threading
		if(client_request_idx > 1)
		{
			client_request_idx = 0;
			while(client_request_idx < MAXPENDING)
			{
				pthread_join(client_request_tid[client_request_idx++], NULL);
			}
			client_request_idx = 0;
		}
	}
	
	return EXIT_SUCCESS;
}