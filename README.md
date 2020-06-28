# Online Multiplayer Tic-tac-toe
This project is a take on the classic game, Tic-tac-toe, which combines a text based interface
with sockets to make a multiplayer game which can be played over the network. The idea stems
from my interest in online multiplayer gaming and serves as a gateway into learning more about
how multiplayers games are structured.

### API's used:
- Sockets API in C  
- Pthread API in C  

### Resource used:
  Practical Guide for Programmers - 2nd Edition  
  Author: Michael J. Donaho, Kenneth L. Calvert
  
### File descriptions
- server.c: This file contains all of the source code for the server.
- tic_tac_toe.c: This file contains all of the source code for client/the game.
- socket.h: This file contains the interface for all of the required functions for both the server and client.
- socket.c: This file contains all of the implementations for the function prototypes defined in socket.h.

### Compilation instructions:
Server: ```gcc server.c socket.c -o server -lpthread -g```  
Client: ```gcc tic_tac_toe.c socket.c -o ttt -lpthread -g```  

Both the server and client was compiled and tested on Lubuntu 18.04 running on a virtual machine.

Note: The -g flag is optional. I only used it for debugging purposes.  

### Usage
Server: `./server <Port Number>`   
Client: `./ttt <Server Address> <Port Number>`
  
### Bug Report 
- Running both the server and client on a Raspberry Pi 3b+ running the Raspberry Pi OS resulted in an undefined behaviour.
- Running the server on a Raspberry Pi 3b+ running the Raspberry Pi OS and the client on Lubuntu 18.04 resulted in the clients
not exiting/disconnecting after one of the clients win.

### Required Additions
- In a scenario where one of the clients disconnects mid-game, a proper handling of closing down the connection for both clients
is required.
