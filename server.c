#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "socket.h"

static const int MAXPENDING = 2; // Maximum number of outstanding connection requests
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void CheckCmdArgs(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("Follow the format: %s <Server Address> <Port Number>", argv[0]);
		exit(0);
	}
}

bool CheckData(char buffer[])
{
	int checkIdx = 1;
	for(; checkIdx < BUFFSIZE; checkIdx++)
	{
		if(buffer[checkIdx] < 0 || buffer[checkIdx] > 2)
			return false;
	}
	return true;
}

void *SendDataToAllClients(void *clientData)
{
	Clients *clients = (Clients *)clientData;
	int id = clients->id;
	printf("Client id: %d, joined the server.\n", id);
	while(1)
	{
		memset(clients->data, 0, BUFFSIZE*sizeof(char));
		ssize_t bytesReceived = ServerReceiveData(clients->descriptors[id], clients->data);
		
		if((FLAGS)clients->data[1] == WON)
		{
			pthread_mutex_lock(&lock);
			{
				char buff[BUFFSIZE];
				memset(&buff, 0, sizeof(buff));
				buff[1] = (char)END;
				int i = 0;
				for(i = 0; i < MAXPENDING; i++)
					Send(clients->descriptors[i], buff);
			}
			pthread_mutex_unlock(&lock);
			break;
		}
		else if(!CheckData(clients->data))
		{
			pthread_mutex_lock(&lock);
			{
				char ErrorBuff[BUFFSIZE];
				memset(&ErrorBuff, 0, sizeof(ErrorBuff));
				ErrorBuff[1] = (char)INVALID_DATA;
				Send(clients->descriptors[id], ErrorBuff);
			}
			pthread_mutex_unlock(&lock);
			continue;
		}
		else if(bytesReceived > 0)
		{
			printf("Received %ld bytes of data from client: %d -> %d,%d\n", bytesReceived, clients->data[0], clients->data[1], clients->data[2]);
			pthread_mutex_lock(&lock);
			{
				int i = 0;
				for(i = 0; i < MAXPENDING; i++)
					Send(clients->descriptors[i], clients->data);
			}
			pthread_mutex_unlock(&lock);
		}
	}
	return NULL;
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
	
	printf("Waiting for connections ...\n");
	
	Clients clients;
	memset(&clients, 0, sizeof(clients));
	
	// Keeps track of the number of clients
	uint8_t ClientCount = 0;
	pthread_t ClientThread[MAXPENDING];
	
	char *DataBuffer = (char *)malloc(BUFFSIZE * sizeof(char));
	clients.data = DataBuffer;
	
	// You want to accept all incoming connections
	while(1)
	{
		SocketAddress ClientAddr;	// Client Address
		// Set the length of the client address structure
		socklen_t ClientAddrLen = sizeof(ClientAddr);
		
		// Wait for client connect
		int sock = accept(SocketDescriptor, (struct sockaddr *)&ClientAddr, &ClientAddrLen);
		
		if(sock < 0)
		{
			perror("accept() failed");
			exit(0);
		}
		
		clients.id = ClientCount;
		
		pthread_mutex_lock(&lock);
		{
			// Send the id back to the client
			char IdBuffer[BUFFSIZE];
			memset(&IdBuffer, 0, sizeof(IdBuffer));
			IdBuffer[0] = ClientCount;
			Send(sock, IdBuffer);
			clients.descriptors[ClientCount] = sock;
		}
		pthread_mutex_unlock(&lock);
		
		char ClientName[INET_ADDRSTRLEN];
		if(inet_ntop(AF_INET, &ClientAddr.sin_addr.s_addr, ClientName, sizeof(ClientName)) != NULL)
			printf("Handling client %s/%d\n", ClientName, ntohs(ClientAddr.sin_port));
		else
			printf("Unable to get client address.\n");
		
		// Instantiate thread to receive and send data to client
		if(pthread_create(&ClientThread[ClientCount++], NULL, SendDataToAllClients, (void *)&clients))
		{
			printf("Failed to instantiate thread for client %d.\n", ClientCount);
		}
		
		if(ClientCount == 2)
		{
			pthread_mutex_lock(&lock);
			{
				// Send the id back to the client
				char Start[BUFFSIZE];
				memset(&Start, 0, sizeof(Start));
				Start[1] = PLAY;
				int i = 0;
				for(; i < MAXPENDING; i++)
					Send(clients.descriptors[i], Start);
				ClientCount = 0;
			}
			pthread_mutex_unlock(&lock);
		}
	}
	
	free(DataBuffer);
	int i = 0;
	for(i = 0; i < MAXPENDING; i++)
		pthread_join(ClientThread[i], NULL);
	
	return EXIT_SUCCESS;
}