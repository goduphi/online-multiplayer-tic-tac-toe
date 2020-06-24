#include "socket.h"

// Param 1: Pass in a char array to hold ip address
// Param 2: pass in the port number
// Param 3: Pass in argv
void SetupIpPort(char ip[], in_port_t *port, char *argv[])
{
	strcpy(ip, argv[1]);
	*port = atoi(argv[2]);
}

// Create a synchronous socket
int CreateSyncSocket()
{
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if(sock < 0)
	{
		perror("socket() failed: ");
		exit(0);
	}
	
	return sock;
}

// Param 1: Pass in the address of the sockaddr_in structure
// Param 2: Pass in a char array inside of which should be an ip address in dotted-quad formatter
// Param 3: Pass the the port number
void ConstructServerAddrPort(struct sockaddr_in *servAddr, char ip[], in_port_t port)
{
	// Prepare the sockaddr_in structure to hold server address
	memset(servAddr, 0, sizeof(*servAddr));
	servAddr->sin_family = AF_INET; // Set the address family to IPv4
	
	// Converts the string representation of the server's Internet address
	// int dotted-quad notation, into a 32-bit binary representation.
	int retVal = inet_pton(AF_INET, ip, &(servAddr->sin_addr.s_addr));
	
	if(retVal == 0)
		printf("inet_pton() failed: Invalid address string.");
	else if(retVal < 0)
		perror("inet_pton() failed: ");
	
	// htons, "host to network short", ensures that the binary value is formatter
	// as required by the API
	servAddr->sin_port = htons(port);
}

// Param 1: Pass in the address of the sockaddr_in structure
// Param 2: Pass the the port number
void ConstructServerPort(struct sockaddr_in *servAddr, in_port_t port)
{
	// Prepare the sockaddr_in structure to hold server address
	memset(servAddr, 0, sizeof(*servAddr));			// Zero out the structure
	servAddr->sin_family = AF_INET; 				// Set the address family to IPv4
	servAddr->sin_addr.s_addr = htonl(INADDR_ANY);	// Any incoming interface
	
	// htons, "host to network short", ensures that the binary value is formatter
	// as required by the API
	servAddr->sin_port = htons(port);
}

// Param 1: Pass in socket descriptor
// Param 2: Pass in a char array that holds the string to send
void Send(int socketDescriptor, char string[])
{
	ssize_t numBytes = send(socketDescriptor, string, BUFFSIZE, 0);
	
	if(numBytes < 0)
	{
		perror("send() failed: ");
	}
	else if(numBytes != BUFFSIZE)
	{
		printf("send() sent an unexpected number of bytes.");
	}
}

// Param 1: Pass in socket descriptor
// Param 2: Pass in a char array in which a string will be returned
void ReceiveData(int socketDescriptor, char string[])
{
	unsigned int totalBytesReceived = 0; // Count the total number of bytes received
	// Receive the length of the data sent
	while(totalBytesReceived < BUFFSIZE)
	{	
		// recv() will block until data is available
		int numBytes = recv(socketDescriptor, string, BUFFSIZE, 0);
		
		if(numBytes < 0)
			perror("recv() failed");
		else if(numBytes == 0)
			printf("recv(), connection close prematurely.");
		
		totalBytesReceived += numBytes;
		string[numBytes] = '\0';
	}
}

ssize_t ServerReceiveData(int socketDescriptor, char string[])
{
	// Receive message from client
	ssize_t numBytesRcvd = recv(socketDescriptor, string, BUFFSIZE, 0);
	if(numBytesRcvd < 0)
		perror("recv() failed");
	return numBytesRcvd;
}
