#include <stdio.h>
#include <stdbool.h>
#include "socket.h"

/*
	0 - represents an empty space
	1 - represents an X
	2 - represents a O
*/

#define BOARD_SIZE 3

// Stores player coordinates
typedef struct Coordinates
{
	int x;
	int y;
} Coordinates;

// Check for row or column
typedef enum CHECK
{
	COL,
	ROW
} CHECK;

// Creates the board
// Param: Pass in a two-dimensional int array
void CreateBoard(int Board[][BOARD_SIZE])
{
	int i, j;
	for(i = 0; i < BOARD_SIZE; i++)
	{
		for(j = 0; j < BOARD_SIZE; j++)
		{
			Board[i][j] = 0;
		}
	}
}

// Prints the board
// Param 1: Pass in the already initialized two-dimensional array
// Param 2: Pass in player character 1 to be populated
// Param 3: Pass in player character 2 to be populated
void PrintBoard(int Board[][BOARD_SIZE], char Player1Char, char Player2Char)
{
	int i, j;
	
	for(i = 0; i < BOARD_SIZE; i++)
	{
		for(j = 0; j < BOARD_SIZE; j++)
		{
			if(Board[i][j] == 0)
			{
				printf(" ");
			}
			else if(Board[i][j] == 1)
			{
				printf("%c", Player1Char);
			}
			else if(Board[i][j] == 2)
			{
				printf("%c", Player2Char);
			}
			
			if(j < BOARD_SIZE - 1)
			{
				printf("|");
			}
		}
		
		printf("\n");
		
		if(i < BOARD_SIZE - 1)
		{
			int k;
			for(k = 0; k < 5; k++)
				printf("-");
			
			printf("\n");
		}
	}
}

// Checks for valid input
// Param: Pass in player Coordinates
bool CheckIfValidInput(Coordinates PlayerInput)
{
	if(PlayerInput.x > 2 && PlayerInput.x < 0 || PlayerInput.y > 2 && PlayerInput.y < 0)
		return false;
	return true;
}

// Checks to see if a value already exists inside the two-dimensional array
// Param 1: Pass in the already initialized two-dimensional array
// Param 2: Pass in the player Coordinates
bool CheckIfFieldAlreadyFilled(int Board[][BOARD_SIZE], Coordinates PlayerInput)
{
	if(Board[PlayerInput.x][PlayerInput.y] != 0)
		return false;
	return true;
}

// Inserts the coordinates into the two-dimensional array
// Param 1: Pass in the already initialized two-dimensional array
// Param 2: Pass in the player Coordinates
// Param 3: Pass in the player counter to keep track of which player it is
bool InsertCoordinates(int Board[][BOARD_SIZE], Coordinates PlayerInput, int InternalPlayerCounter)
{
	if(CheckIfFieldAlreadyFilled(Board, PlayerInput) && CheckIfValidInput(PlayerInput))
	{
		if(InternalPlayerCounter % 2 == 0)
			Board[PlayerInput.x][PlayerInput.y] = 2;
		else
			Board[PlayerInput.x][PlayerInput.y] = 1;
		
		return true;
	}
	else
	{
		return false;
	}
}

// Checks the row/col if the player already has 3 consecutive characters
// Param 1: Pass in the already initialized two-dimensional array
// Param 2: Pass in the address to who the player is, that is the player number
// Param 3: Pass in the enum value to say to check the row/col
bool CheckRowCol(int Board[][BOARD_SIZE], int *PlayerNumber, CHECK check)
{
	int i = 0;
	int j = 0;
	
	for(i = 0; i < BOARD_SIZE; i++)
	{
		int first_idx_val = Board[i][0];
		
		if(first_idx_val == 0)
		{
			break;
		}
		
		for(j = 1; j < BOARD_SIZE; j++)
		{
			switch(check)
			{
				case ROW:
					if(first_idx_val != Board[i][j])
					{
						return false;
					}
				break;
				case COL:
					if(first_idx_val != Board[j][i])
					{
						return false;
					}
				break;
			}
		}
		
		if(j == BOARD_SIZE)
		{
			*PlayerNumber = first_idx_val;
			return true;
		}
	}
	
	return false;
}

// Checks the diagonal going from 0,0 to 2,2
// Param 1: Pass in the already initialized two-dimensional array
// Param 2: Pass in the address to who the player is, that is the player number
bool CheckLRDiag(int Board[][BOARD_SIZE], int *PlayerNumber)
{
	int i = 0;
	
	int first_idx_val;
	
	for(i = 1; i < BOARD_SIZE; i++)
	{
		if(i == 1)
		{
			first_idx_val = Board[i - 1][0];
		}
		
		if(first_idx_val == 0)
		{
			break;
		}
		
		if(first_idx_val != Board[i][i])
		{
			return false;
		}
		
	}
	
	if(i == BOARD_SIZE)
	{
		*PlayerNumber = first_idx_val;
		return true;
	}
	
	return false;
}

// Checks the diagonal going from 0,2 to 2,0
// Param 1: Pass in the already initialized two-dimensional array
// Param 2: Pass in the address to who the player is, that is the player number
bool CheckRLDiag(int Board[][BOARD_SIZE], int *PlayerNumber)
{
	int i = 0;
	int j = 1;
	
	int first_idx_val;
	
	for(i = BOARD_SIZE - 2; i >= 0; i--)
	{
		if(i == 1)
			first_idx_val = Board[0][BOARD_SIZE - 1];
		
		if(first_idx_val == 0)
		{
			return false;
		}
		
		if(first_idx_val != Board[j][i])
		{
			return false;
		}
		
		j++;
	}
	
	if(j == BOARD_SIZE)
	{
		*PlayerNumber = first_idx_val;
		return true;
	}
	return false;
}

void CheckCmdArgs(int argc, char *argv[])
{
	if(argc < 2 || argc > 3)
	{
		printf("Follow the format: %s <Server Address> <Port Number>", argv[0]);
		exit(0);
	}
}

void ConvertToString(char *ReturnStrArray, Coordinates coordinates)
{
	memset(ReturnStrArray, 0, sizeof(ReturnStrArray));
	sprintf(ReturnStrArray, "%d,%d", coordinates.x, coordinates.y);
}

void ConvertToCoordinates(char *ClientString, Coordinates *coordinates)
{
	sscanf(ClientString, "%d,%d", &coordinates->x, &coordinates->y);
}

int main(int argc, char *argv[])
{
	
	// Check to see if the user put in the ip address and port number of the server
	CheckCmdArgs(argc, argv);
	
	// This section of the code is responsible for connecting to the server
	// Initialize the server ip and port
	char ServerIp[20];
	in_port_t Port = 0;
	SetupIpPort(ServerIp, &Port, argv);
	
	// Create a synchronous socket
	int SocketDescriptor = CreateSyncSocket();
	
	// Prepare address of the server
	SocketAddress ServAddr;
	ConstructServerAddrPort(&ServAddr, ServerIp, Port);
	
	// Establish a connection with the server
	if(connect(SocketDescriptor, (struct sockaddr *)&ServAddr, sizeof(ServAddr)) < 0)
		perror("connect() failed");
	
	int MainBoard[BOARD_SIZE][BOARD_SIZE];
	int InternalPlayerCounter = 1;
	int PlayerNumber = 0;
	
	CreateBoard(MainBoard);
	
	printf("WELCOME TO TIC-TAC-TOE\n\n");
	
	char Player1Char;
	char Player2Char;
	
	Coordinates PlayerInput;
	
	printf("Choose the character to be printed (Format: X O) ");

	scanf(" %c", &Player1Char);
	scanf(" %c", &Player2Char);
	
	PrintBoard(MainBoard, Player1Char, Player2Char);
	
	// Game loop
	while(1)
	{
		char StringCoordinate[BUFFSIZE];
		memset(StringCoordinate, 0, sizeof(StringCoordinate));
		
		printf("Enter coordinates in the format x,y ");

		getchar();
		
		scanf("%d,%d", &PlayerInput.x, &PlayerInput.y);
		
		ConvertToString(StringCoordinate, PlayerInput);

		Send(SocketDescriptor, StringCoordinate);
		
		// Handle incoming data
		Coordinates IncomingCoordinates;
		memset(StringCoordinate, 0, sizeof(StringCoordinate));
		//memset(StringCoordinate, 0, sizeof(StringCoordinate));
		ReceiveData(SocketDescriptor, StringCoordinate);
		
		printf("%s\n", StringCoordinate);
		
		ConvertToCoordinates(StringCoordinate, &IncomingCoordinates);
		
		if(InsertCoordinates(MainBoard, IncomingCoordinates, InternalPlayerCounter))
			InternalPlayerCounter++;
		else
			printf("Player %d goes again.\n", (InternalPlayerCounter%2 == 0 ? 2 : 1 ));

		PrintBoard(MainBoard, Player1Char, Player2Char);
		
		if(CheckRowCol(MainBoard, &PlayerNumber, ROW) || CheckRowCol(MainBoard, &PlayerNumber, COL) ||
		   CheckRLDiag(MainBoard, &PlayerNumber) || CheckLRDiag(MainBoard, &PlayerNumber))
		{
			printf("Player %d WINS!!!\n", PlayerNumber);
			break;
		}
	}
	
	close(SocketDescriptor);
	
	return 0;
}
