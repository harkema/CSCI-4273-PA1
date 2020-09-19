# CSCI-4273-PA1
Kiana Harkema
106102554

Program is organized in the following directories:

Server
  Makefile
  udp_server.c
 Client
  Makefile
  udp_client.c
  foo1
  foo2
  foo3
 
 
 To start the program, begin by compiling both the server and client executable by running the following commands in their repsective folders:
 
 make clean
 make all
 
 Once the exectuables are created, start the server:
 
 ./server <port #>
 
 Then start the client:
 
 ./client <localhost or remote server ip> <port #>
 
 The following commands can be run from the client:
 
 get <file name>
 put <file name>
 delete <file name>
 ls 
 exit
 
 If the command is invalid, the server will alert the host. It will do the same for any error. To end both the client and server session, type exit. 

