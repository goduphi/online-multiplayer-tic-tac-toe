# Multiplayer Tic-Tac-Toe in C
## Author: Sarker Nadir Afridi Azmi
## Resource used:
  Practical Guide for Programmers - 2nd Edition  
  Author: Michael J. Donaho, Kenneth L. Calvert

### Compilation instructions:
  Client: ```gcc tic_tac_toe.c socket.c -o ttt -g```  
  Server: ```gcc server.c socket.c -o server -lpthread -g``` 
  
  Note: The -g flag is optional. I only used it for debugging purposes.

### API's used:
  Sockets API in C  
  pthread API in C  
  
### Bugs  
  Both server and client require proper signal handling to handle abrubt disconnects.  
  Client connections do not terminate when any one of clients win the game.
