# Multiplayer Tic-Tac-Toe in C
## Author: Sarker Nadir Afridi Azmi

### Compilation instructions:
  Client: ```gcc tic_tac_toe.c socket.c -o ttt -g```  
  Server: ```gcc server.c socket.c -o server -lpthread -g``` 
  
  Note: The -g flag is optional. I only used it for debugging purposes.

### API's used:
  Sockets API in C  
  pthread API in C  
  
### Yet to implement
- [ ] Send data from one client to the other client.
  
### Note
  All of the code was written in C and compiled on Linux using gcc. A lot of the code is not portable.
